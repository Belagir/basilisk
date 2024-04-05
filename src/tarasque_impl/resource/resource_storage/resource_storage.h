
#ifndef __TARASQUE_RESOURCE_STORAGE_H__
#define __TARASQUE_RESOURCE_STORAGE_H__

#include <tarasque.h>

#include "../../common.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

typedef struct resource_storage_data resource_storage_data;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

resource_storage_data *resource_storage_data_create(const char *str_storage_name, allocator alloc);

void resource_storage_data_destroy(resource_storage_data **storage_data, allocator alloc);

// -------------------------------------------------------------------------------------------------

bool resource_storage_check(resource_storage_data *storage_data, const char *str_path, allocator alloc);

void *resource_storage_data_get(resource_storage_data *storage_data, const char *str_path, size_t *out_size, allocator alloc);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

#endif
