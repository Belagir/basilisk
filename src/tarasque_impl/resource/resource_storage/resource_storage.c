
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
 * @param alloc
 */
void resource_storage_data_sync_from_path(resource_storage_data *storage_data, const char *str_path, allocator alloc)
{
    FILE *found_file = NULL;
    resource_item *found_item = NULL;
    u32 str_path_hash = 0u;
    size_t item_index = 0u;

    if (!storage_data || *str_path) {
        return;
    }

    found_file = fopen(str_path, "r");

    if (!found_file) {
        return;
    }

    str_path_hash = hash_jenkins_one_at_a_time((const byte *) str_path, c_string_length(str_path, false), 0u);

    if (!sorted_range_find_in(RANGE_TO_ANY(storage_data->items), &hash_compare_doubleref, &str_path_hash, &item_index)) {
        storage_data->items = range_ensure_capacity(alloc, RANGE_TO_ANY(storage_data->items), 1);
        item_index = sorted_range_insert_in(RANGE_TO_ANY(storage_data->items), &hash_compare_doubleref, &(resource_item) { .str_path_hash = str_path_hash });
    }

    found_item = storage_data->items->data + item_index;

    if (found_item->data) {
        alloc.free(alloc, found_item->data);
        found_item->data_size = 0u;
    }

    // copy file in the item's data

    fclose(found_file);
}

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
