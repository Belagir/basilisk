/**
 * @file resource_storage.c
 * @author gabriel ()
 * @brief Implementation file for the resource file deserialization object.
 * This file centralizes all the filesystem interactions and conditional compilation needed by the resource management module.
 * @version 0.1
 * @date 2024-04-06
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <ustd/sorting.h>

#include <stdio.h>
#include <sys/stat.h>

#include "basilisk_resource_storage.h"


// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @typedef file_data_array
 * @brief Shorthand symbol for the bytes found in a file. Used to quickly allocate buffers to read files into.
 */
typedef RANGE(byte) file_data_array;

/**
 * @brief Resource header presenting information about a resource in a storage file.
 * This layout can be found directly in the storage file and is also used in the program memory.
 * Needs to subtype the `u32` type for ordering.
 */
typedef struct resource_item_header {
    /** Hash of the path that led to the original resource file and used to access a resource from user code. */
    u32 str_path_hash;
    /** Size of the resource, in bytes. */
    size_t data_size;
} resource_item_header;

/**
 * @brief Describes a codebase-exclusive data layout for a resource found in a file.
 */
typedef struct resource_item_deserialized {
    /** Resource information pulled from the storage file. */
    resource_item_header header;

    /** Pointer to some allocated memory containing the resource's bytes. */
    void *data;
} resource_item_deserialized;

/**
 * @brief Describes a set of resources found in a single storage file.
 * Needs to subtype the `u32` type for ordering.
 */
typedef struct resource_storage {
    /** Hash of the storage data name. Used for ordering collections of storages. */
    u32 storage_name_hash;

    /** Flags that the storage has loaded the resources from its associated storage file. */
    bool is_loaded;
    /** Static string representing the path to the storage file associated to this storage object. */
    const char *file_path; // TODO (low prio, all code paths require static strings) : use an identifier or path or RANGE(char)
    /** Collection of resources, ordered by their hash. */
    RANGE(resource_item_deserialized) *items;

    /** Collection of entities that are using the resources of this storage. */
    RANGE(basilisk_entity *) *supplicants;
} resource_storage;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* Loads the contents of a file into a buffer (a range of bytes) object. */
static bool file_data_array_from(const char *str_path, file_data_array **dest, allocator alloc);

/* Loads resources from a storage file into a storage object if the object was not marked as loaded. */
static void resource_storage_load(resource_storage *storage, allocator alloc);

/* De-allocates memory used to store resources, removing resources data from the object. */
static void resource_storage_unload(resource_storage *storage, allocator alloc);

/* Copies the contents of a file at the end of a resource storage file. */
static bool storage_file_append(const char *storage_file_path, const char *res_path, allocator alloc);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Creates a data storage object meant to load, store, service and release resources present in a single storage file.
 * In nominal (development -- with no compilation switch) mode, calling this function will not only create the object, but also
 * create or empty the given storage file. On failure to do this, the function will abort the object creation, and will return NULL.
 * With BASILISK_RELEASE set, this function will just check that the given file can be opened in read mode. On failure, the
 * object will not be created and the function will return NULL.
 *
 * Calling this function will not load the resources present in the storage file.
 *
 * @param[in] str_storage_path Static string representing a path to the storage file. The object will retain a pointer to the string for following operations.
 * @param[inout] alloc Allocator used to request memory for the object creation.
 * @return resource_storage *
 */
resource_storage *resource_storage_create(const char *str_storage_path, allocator alloc)
{
    resource_storage *new_storage = NULL;
    FILE *storage_file = NULL;

    if (!str_storage_path) {
        return NULL;
    }

#ifndef BASILISK_RELEASE
    (void) mkdir(BASILISK_RESOURCE_STORAGES_FOLDER, S_IRWXU);
    storage_file = fopen(str_storage_path, "w");
#else
    storage_file = fopen(str_storage_path, "r");
#endif

    if (!storage_file) {
        return NULL;
    }
    fclose(storage_file);

    new_storage = alloc.malloc(alloc, sizeof(*new_storage));
    if (new_storage) {

        *new_storage = (resource_storage) {
                .storage_name_hash = hash_jenkins_one_at_a_time((const byte *) str_storage_path, c_string_length(str_storage_path, false), 0u),
                .file_path = str_storage_path,
                .items = range_create_dynamic(alloc, sizeof(*new_storage->items->data), BASILISK_COLLECTIONS_START_LENGTH),
                .supplicants = range_create_dynamic(alloc, sizeof(*new_storage->supplicants->data), BASILISK_COLLECTIONS_START_LENGTH),
        };
    }

    return new_storage;
}

/**
 * @brief Releases memory taken by a resource storage and nullifies the given pointer.
 * All resources fetched from the supplied storage object are invalidated.
 *
 * @param[inout] storage_data Target storage data to destroy.
 * @param[inout] alloc Allocator used to release all memory.
 */
void resource_storage_destroy(resource_storage **storage_data, allocator alloc)
{
    if (!storage_data || !*storage_data) {
        return;
    }

    resource_storage_unload(*storage_data, alloc);

    range_destroy_dynamic(alloc, &RANGE_TO_ANY((*storage_data)->supplicants));
    range_destroy_dynamic(alloc, &RANGE_TO_ANY((*storage_data)->items));

    alloc.free(alloc, *storage_data);
    *storage_data = NULL;
}

// -------------------------------------------------------------------------------------------------

/**
 * @brief Checks that a resource (by its path) exists in a storage file.
 * In nominal (development -- with no compilation switch) mode, this function will try to load the file present at the given path
 * and append it to the storage file associated to the storage object. With BASILISK_RELEASE set, this step is skipped.
 * Then, the function will check that the resource is present in the storage file and return true if it finds it and false if not.
 *
 * @param[inout] storage_data Storage object.
 * @param[in] str_path Path to the resource used to either update or identify the checked resource.
 * @param[inout] alloc Allocator used for the eventual file reading.
 * @return bool
 */
bool resource_storage_check(resource_storage *storage_data, const char *str_path, allocator alloc)
{
    FILE *storage_file = NULL;
    resource_item_header item_header = { 0u };
    u32 str_path_hash = 0u;
    bool found = false;

    if (!storage_data || !str_path) {
        return false;
    }

#ifndef BASILISK_RELEASE
    if (!storage_file_append(storage_data->file_path, str_path, alloc)) {
        return false;
    }
#endif

    str_path_hash = hash_jenkins_one_at_a_time((const byte *) str_path, c_string_length(str_path, false), 0u);

    storage_file = fopen(storage_data->file_path, "r");
    if (!storage_file) {
        return false;
    }

    while (!found && !feof(storage_file)) {
        (void) fread(&item_header, sizeof(item_header), 1, storage_file);
        found = (item_header.str_path_hash == str_path_hash);
        fseek(storage_file, (long int) item_header.data_size, SEEK_CUR);
    }

    fclose(storage_file);

    return found;
}

// -------------------------------------------------------------------------------------------------

/**
 * @brief Returns the resource data and size associated to a path in a storage object. If the storage has no supplicant
 * entity, the storage object had not have loaded its associated storage file yet, and will return NULL, regardless of
 * the resource existence.
 *
 * @param[inout] storage_data Target storage data the resource was declared to.
 * @param[in] str_path Path to the resource file, used to identify the resource.
 * @param[out] out_size Outgoing size of the returned data, in bytes.
 * @return void *
 */
void *resource_storage_get(resource_storage *storage_data, const char *str_path, size_t *out_size)
{
    u32 str_path_hash = 0u;
    size_t data_index = 0u;

    if (out_size) {
        *out_size = 0u;
    }

    if (!storage_data || !str_path) {
        return NULL;
    }

    str_path_hash = hash_jenkins_one_at_a_time((const byte *) str_path, c_string_length(str_path, false), 0u);

    if (sorted_range_find_in(RANGE_TO_ANY(storage_data->items), &hash_compare, &str_path_hash, &data_index)) {
        if (out_size) {
            *out_size = storage_data->items->data[data_index].header.data_size;
        }
        return storage_data->items->data[data_index].data;
    }

    return NULL;
}

/**
 * @brief Adds an entity as a user of a storage. If the storage had no previous other supplicant entity, it
 * will load the resources present in its associated file, if it exists.
 *
 * Supplicants are used to track the usage of a storage, and detect simply when to load and unload the resources
 * present in a storage file.
 *
 * @param[inout] storage_data Target storage the supplicant entity is registered to.
 * @param[in] entity Entity marked as using the storage.
 * @param[inout] alloc Allocator used for the eventual range allocation and resource loading.
 */
void resource_storage_add_supplicant(resource_storage *storage_data, basilisk_entity *entity, allocator alloc)
{
    if (!storage_data || !entity) {
        return;
    }

    if (sorted_range_find_in(RANGE_TO_ANY(storage_data->supplicants), &raw_pointer_compare, &entity, NULL)) {
        return;
    }

    if (storage_data->supplicants->length == 0) {
        resource_storage_load(storage_data, alloc);
    }

    storage_data->supplicants = range_ensure_capacity(alloc, RANGE_TO_ANY(storage_data->supplicants), 1);
    sorted_range_insert_in(RANGE_TO_ANY(storage_data->supplicants), &raw_pointer_compare, &entity);
}

/**
 * @brief Removes an entity as a storage user. If this entity was the last one to be a supplicant to the storage,
 * all resources loaded from the filesystem are released from memory.
 *
 * @param[inout] storage_dataTarget storage the supplicant entity is unregistered from.
 * @param[in] entity Entity withdrawing its usage from the storage.
 * @param[inout] alloc Allocator used to unlaod the resources.
 */
void resource_storage_remove_supplicant(resource_storage *storage_data, basilisk_entity *entity, allocator alloc)
{
    if (!storage_data || !entity) {
        return;
    }

    sorted_range_remove_from(RANGE_TO_ANY(storage_data->supplicants), &raw_pointer_compare, &entity);

    if (storage_data->supplicants->length == 0) {
        resource_storage_unload(storage_data, alloc);
    }
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Reads the content of a file and copies it into a destination range. The function will return
 * true on success, and false otherwise.
 *
 * @param[in] str_path Path to the file on the filesystem.
 * @param[out] dest Destination range of bytes, might be reallocated by the function.
 * @param[in] alloc Allocator used to extend the range.
 * @return bool
 */
static bool file_data_array_from(const char *str_path, file_data_array **dest, allocator alloc)
{
    FILE *file = NULL;
    struct stat file_info = { 0u };

    if (!str_path || !dest || !*dest) {
        return false;
    }

    file = fopen(str_path, "r");

    if (!file) {
        return false;
    }

    stat(str_path, &file_info);

    range_clear(RANGE_TO_ANY((*dest)));
    *dest = range_ensure_capacity(alloc, RANGE_TO_ANY((*dest)), (size_t) file_info.st_size);
    (*dest)->length = fread((*dest)->data, 1u, (size_t) file_info.st_size, file);

    fclose(file);

    return true;
}

/**
 * @brief Adds the entries from a storage file into its storage object, if the storage object was set as not loaded.
 *
 * @param[inout] storage Target storage to populate.
 * @param[in] alloc Allocator used to create memory to store the resources found in the file.
 */
static void resource_storage_load(resource_storage *storage, allocator alloc)
{
    FILE *storage_file = NULL;
    resource_item_deserialized item = { 0u };

    if (!storage || storage->is_loaded) {
        return;
    }

    storage_file = fopen(storage->file_path, "r");
    if (!storage_file) {
        return;
    }

    while (!feof(storage_file) && !ferror(storage_file)) {
        if (fread(&item.header, 1, sizeof(item.header), storage_file) == sizeof(item.header)) {
            item.data = alloc.malloc(alloc, item.header.data_size);
            fread(item.data, item.header.data_size, 1, storage_file);

            storage->items = range_ensure_capacity(alloc, RANGE_TO_ANY(storage->items), 1);
            sorted_range_insert_in(RANGE_TO_ANY(storage->items), &hash_compare, &item);
        }
    }

    fclose(storage_file);

    storage->is_loaded = true;
}

/**
 * @brief removes all entries from a storage object, if the storage object was set as laoded.
 *
 * @param[inout] storage Target storage to empty.
 * @param[in] alloc Allocator used to release the resources' memory.
 */
static void resource_storage_unload(resource_storage *storage, allocator alloc)
{
    if (!storage || !storage->is_loaded) {
        return;
    }

    for (size_t i = 0u ; i < storage->items->length ; i++) {
        alloc.free(alloc, storage->items->data[i].data);
    }
    range_clear(RANGE_TO_ANY(storage->items));

    storage->is_loaded = false;
}

/**
 * @brief Appends the contentes of a file at the end of a storage file, after a header containing the hash of the path
 * to the resource file and the number of appended bytes (excluding the header).
 * The function will return true if the operation succeeded, and false otherwise.
 *
 * @param storage_file_path Path to the target storage file.
 * @param res_path Path to the resource file.
 * @param alloc Allocator used to create a buffer to read the file.
 * @return bool
 */
static bool storage_file_append(const char *storage_file_path, const char *res_path, allocator alloc)
{
    file_data_array *resource_file_data = NULL;
    FILE *storage_file = NULL;
    u32 str_path_hash = 0u;

    if (!storage_file_path || !res_path) {
        return false;
    }

    str_path_hash = hash_jenkins_one_at_a_time((const byte *) res_path, c_string_length(res_path, false), 0u);

    // fetch raw data from the target file
    resource_file_data = range_create_dynamic(alloc, sizeof(*resource_file_data->data), 1u);

    if (!file_data_array_from(res_path, &resource_file_data, alloc)) {
        range_destroy_dynamic(alloc, &RANGE_TO_ANY(resource_file_data));
        return false;
    }

    // append resource file content at str_path in storage file
    storage_file = fopen(storage_file_path, "a");
    if (!storage_file) {
        range_destroy_dynamic(alloc, &RANGE_TO_ANY(resource_file_data));
        return false;
    }

    fwrite(&(resource_item_header) { .str_path_hash = str_path_hash, .data_size = resource_file_data->length }, sizeof(resource_item_header), 1, storage_file);
    fwrite(resource_file_data->data, resource_file_data->length, 1, storage_file);
    fflush(storage_file);

    fclose(storage_file);
    range_destroy_dynamic(alloc, &RANGE_TO_ANY(resource_file_data));

    return true;
}
