
#ifndef __TARASQUE_RESOURCE_H__
#define __TARASQUE_RESOURCE_H__

#include <tarasque.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

typedef struct resource_manager resource_manager;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

resource_manager *resource_manager_create(allocator alloc);

void resource_manager_destroy(resource_manager **res_manager, allocator alloc);

// -------------------------------------------------------------------------------------------------

void *resource_manager_fetch(resource_manager *res_manager, const char *str_storage, const char *str_res_path, allocator alloc);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

#endif
