
#include <ustd/sorting.h>

#include "resource.h"

#include "resource_storage/resource_storage.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

typedef struct resource_manager {
    RANGE(resource_storage *) *storages;
} resource_manager;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param alloc
 * @return
 */
resource_manager *resource_manager_create(allocator alloc)
{
    resource_manager *new_res_manager = NULL;

    new_res_manager = alloc.malloc(alloc, sizeof(*new_res_manager));

    if (new_res_manager) {
        *new_res_manager = (resource_manager) {
            .storages = range_create_dynamic(alloc, sizeof(*new_res_manager->storages->data), TARASQUE_COLLECTIONS_START_LENGTH),
        };
    }

    return new_res_manager;
}

/**
 * @brief
 *
 * @param res_manager
 * @param alloc
 */
void resource_manager_destroy(resource_manager **res_manager, allocator alloc)
{
    if (!res_manager || !*res_manager) {
        return;
    }

    for (size_t i = 0u ; i < (*res_manager)->storages->length ; i++) {
        resource_storage_destroy((*res_manager)->storages->data + i, alloc);
    }

    range_destroy_dynamic(alloc, &RANGE_TO_ANY((*res_manager)->storages));

    alloc.free(alloc, *res_manager);
    *res_manager = NULL;
}

// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param res_manager
 * @param str_storage
 * @param str_res_path
 * @param alloc
 * @return
 */
bool resource_manager_check(resource_manager *res_manager, const char *str_storage, const char *str_res_path, allocator alloc)
{
    size_t found_storage_index = 0u;
    u32 storage_name_hash = 0u;
    resource_storage *new_storage = NULL;

    if (!res_manager) {
        return false;
    }

    storage_name_hash = hash_jenkins_one_at_a_time((const byte *) str_storage, c_string_length(str_storage, false), 0u);

    if (!sorted_range_find_in(RANGE_TO_ANY(res_manager->storages), &hash_compare_doubleref, &(u32 *) { &storage_name_hash }, &found_storage_index)) {
        new_storage = resource_storage_create(str_storage, alloc);
        if (new_storage) {
            found_storage_index = sorted_range_insert_in(RANGE_TO_ANY(res_manager->storages), &hash_compare_doubleref, &new_storage);
        }
    }

    return resource_storage_check(res_manager->storages->data[found_storage_index], str_res_path, alloc);
}

/**
 * @brief
 *
 * @param res_manager
 * @param str_storage
 * @param str_res_path
 * @param alloc
 * @return
 */
void *resource_manager_fetch(resource_manager *res_manager, const char *str_storage, const char *str_res_path, size_t *out_size, allocator alloc)
{
    size_t found_storage_index = 0u;
    u32 storage_name_hash = 0u;

    if (!res_manager || !str_res_path || !str_storage) {
        return NULL;
    }

    storage_name_hash = hash_jenkins_one_at_a_time((const byte *) str_storage, c_string_length(str_storage, false), 0u);

    if (sorted_range_find_in(RANGE_TO_ANY(res_manager->storages), &hash_compare_doubleref, &(u32 *) { &storage_name_hash }, &found_storage_index)) {
        return resource_storage_get(res_manager->storages->data[found_storage_index], str_res_path, out_size, alloc);
    }

    return NULL;
}

/**
 * @brief
 *
 * @param res_manager
 * @param str_storage
 * @param entity
 * @param alloc
 */
void resource_manager_add_supplicant(resource_manager *res_manager, const char *str_storage, tarasque_entity *entity, allocator alloc)
{
    size_t found_storage_index = 0u;
    u32 storage_name_hash = 0u;

    if (!res_manager || !entity || !str_storage) {
        return;
    }

    storage_name_hash = hash_jenkins_one_at_a_time((const byte *) str_storage, c_string_length(str_storage, false), 0u);

    if (sorted_range_find_in(RANGE_TO_ANY(res_manager->storages), &hash_compare_doubleref, &(u32 *) { &storage_name_hash }, &found_storage_index)) {
        resource_storage_add_supplicant(res_manager->storages->data[found_storage_index], entity, alloc);
    }
}

/**
 * @brief
 *
 * @param res_manager
 * @param entity
 * @param alloc
 */
void resource_manager_remove_supplicant(resource_manager *res_manager, tarasque_entity *entity, allocator alloc)
{
    if (!res_manager) {
        return;
    }

    for (size_t i = 0u ; i < res_manager->storages->length ; i++) {
        resource_storage_remove_supplicant(res_manager->storages->data[i], entity, alloc);
    }
}
