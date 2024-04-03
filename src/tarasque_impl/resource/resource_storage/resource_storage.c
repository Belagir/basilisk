
#include "resource_storage.h"

typedef struct resource_item {
    u32 path_hash;

    u32 checksum;
    size_t data_length;
    byte data[];
} resource_item;

typedef struct resource_storage_data {
    u32 storage_name_hash;
    RANGE(resource_item *) *items;
} resource_storage_data;

resource_storage_data *resource_storage_data_create(const char *str_storage_name, allocator alloc)
{
    return NULL;
}

void resource_storage_data_destroy(resource_storage_data **storage_data, allocator alloc)
{
    return;
}

void resource_storage_data_sync_from_path(resource_storage_data *storage_data, const char *str_path, allocator alloc)
{
    return;
}

void *resource_storage_data_get(resource_storage_data *storage_data, const char *str_path)
{
    return NULL;
}
