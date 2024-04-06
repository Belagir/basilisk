
#include <ustd/sorting.h>

#include <stdio.h>
#include <sys/stat.h>

#include "resource_storage.h"


// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 */
typedef RANGE(byte) file_data_array;

/**
 * @brief
 *
 */
typedef struct resource_item_header {
    u32 str_path_hash;
    size_t data_size;
} resource_item_header;

/**
 * @brief
 *
 */
typedef struct resource_item_deserialized {
    resource_item_header header;

    void *data;
} resource_item_deserialized;

/**
 * @brief
 *
 */
typedef struct resource_storage_data {
    u32 storage_name_hash;

    bool is_loaded;
    // TODO (low prio, all code paths require static strings) : use an identifier or path or RANGE(char)
    const char *file_path;
    RANGE(resource_item_deserialized) *items;
} resource_storage_data;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/*  */
static file_data_array *file_data_array_from(const char *str_path, allocator alloc);

/*  */
static void resource_storage_data_reload(resource_storage_data *storage, allocator alloc);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param str_storage_name
 * @param alloc
 * @return
 */
resource_storage_data *resource_storage_data_create(const char *str_storage_name, allocator alloc)
{
    resource_storage_data *new_storage = NULL;
    FILE *storage_file = NULL;

    if (!str_storage_name) {
        return NULL;
    }

#ifndef TARASQUE_RELEASE
    (void) mkdir(TARASQUE_RESOURCE_STORAGES_FOLDER, S_IRWXU);
    storage_file = fopen(str_storage_name, "w");
#else
    storage_file = fopen(str_storage_name, "r");
#endif

    if (!storage_file) {
        return NULL;
    }
    fclose(storage_file);

    new_storage = alloc.malloc(alloc, sizeof(*new_storage));
    if (new_storage) {

        *new_storage = (resource_storage_data) {
                .storage_name_hash = hash_jenkins_one_at_a_time((const byte *) str_storage_name, c_string_length(str_storage_name, false), 0u),
                .file_path = str_storage_name,
                .items = range_create_dynamic(alloc, sizeof(*new_storage->items->data), TARASQUE_COLLECTIONS_START_LENGTH),
        };
    }

    return new_storage;
}

/**
 * @brief
 *
 * @param storage_data
 * @param alloc
 */
void resource_storage_data_destroy(resource_storage_data **storage_data, allocator alloc)
{
    if (!storage_data || !*storage_data) {
        return;
    }

    for (size_t i = 0u ; i < (*storage_data)->items->length ; i++) {
        alloc.free(alloc, (*storage_data)->items->data[i].data);
    }

    range_destroy_dynamic(alloc, &RANGE_TO_ANY((*storage_data)->items));

    alloc.free(alloc, *storage_data);
    *storage_data = NULL;
}

// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param storage_data
 * @param str_path
 * @param alloc
 * @return
 */
bool resource_storage_check(resource_storage_data *storage_data, const char *str_path, allocator alloc)
{
    FILE *storage_file = NULL;
    resource_item_header item_header = { 0u };
    u32 str_path_hash = 0u;
    bool found = false;

    if (!storage_data || !str_path) {
        return false;
    }

    str_path_hash = hash_jenkins_one_at_a_time((const byte *) str_path, c_string_length(str_path, false), 0u);

#ifndef TARASQUE_RELEASE
    // fetch raw data from the target file
    file_data_array *resource_file_data = file_data_array_from(str_path, alloc);
    if (!resource_file_data) {
        return false;
    }

    // append resource file content at str_path in storage file
    storage_file = fopen(storage_data->file_path, "a");
    if (!storage_file) {
        range_destroy_dynamic(alloc, &RANGE_TO_ANY(resource_file_data));
        return false;
    }

    fwrite(&(resource_item_header) { .str_path_hash = str_path_hash, .data_size = resource_file_data->length }, sizeof(resource_item_header), 1, storage_file);
    fwrite(resource_file_data->data, resource_file_data->length, 1, storage_file);
    fflush(storage_file);

    fclose(storage_file);
    range_destroy_dynamic(alloc, &RANGE_TO_ANY(resource_file_data));
#endif

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
 * @brief
 *
 * @param storage_data
 * @param str_path
 * @return
 */
void *resource_storage_data_get(resource_storage_data *storage_data, const char *str_path, size_t *out_size, allocator alloc)
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

    resource_storage_data_reload(storage_data, alloc);

    if (sorted_range_find_in(RANGE_TO_ANY(storage_data->items), &hash_compare, &str_path_hash, &data_index)) {
        if (out_size) {
            *out_size = storage_data->items->data[data_index].header.data_size;
        }
        return storage_data->items->data[data_index].data;
    }

    return NULL;
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param str_path
 * @param alloc
 * @return
 */
static file_data_array *file_data_array_from(const char *str_path, allocator alloc)
{
    FILE *file = NULL;
    struct stat file_info = { 0u };
    file_data_array *contents = { 0u };

    if (!str_path) {
        return NULL;
    }

    file = fopen(str_path, "r");

    if (!file) {
        return NULL;
    }

    stat(str_path, &file_info);

    contents = range_create_dynamic(alloc, sizeof(*contents->data), (size_t) file_info.st_size);
    contents->length = fread(contents->data, 1u, (size_t) file_info.st_size, file);

    fclose(file);

    return contents;
}

/**
 * @brief
 *
 * @param storage
 * @param alloc
 */
static void resource_storage_data_reload(resource_storage_data *storage, allocator alloc)
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
            sorted_range_insert_in(RANGE_TO_ANY(storage->items), &hash_compare, &item);
        }
    }

    fclose(storage_file);

    storage->is_loaded = true;
}
