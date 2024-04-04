
#include <ustd/sorting.h>

#include <stdio.h>

#include "resource_storage.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 */
typedef struct resource_item {
    u32 str_path_hash;

    size_t data_size;
    void *data;
} resource_item;

/**
 * @brief
 *
 */
typedef struct resource_storage_data {
    u32 storage_name_hash;

    RANGE(resource_item) *items;
} resource_storage_data;

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

    if (!str_storage_name) {
        return NULL;
    }

    new_storage = alloc.malloc(alloc, sizeof(*new_storage));

    if (new_storage) {
        *new_storage = (resource_storage_data) {
                .storage_name_hash = hash_jenkins_one_at_a_time((const byte *) str_storage_name, c_string_length(str_storage_name, false), 0u),
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
 * @return
 */
void *resource_storage_data_get(resource_storage_data *storage_data, const char *str_path)
{
    if (!storage_data || !str_path) {
        return NULL;
    }

    u32 str_path_hash = hash_jenkins_one_at_a_time((const byte *) str_path, c_string_length(str_path, false), 0u);
    size_t data_index = 0u;

    if (sorted_range_find_in(RANGE_TO_ANY(storage_data->items), &hash_compare_doubleref, &str_path_hash, &data_index)) {
        return storage_data->items->data[data_index].data;
    }

    return NULL;
}
