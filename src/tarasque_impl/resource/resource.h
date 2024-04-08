
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

bool resource_manager_check(resource_manager *res_manager, const char *str_storage, const char *str_res_path, allocator alloc);

void *resource_manager_fetch(resource_manager *res_manager, const char *str_storage, const char *str_res_path, size_t *out_size, allocator alloc);

void resource_manager_add_supplicant(resource_manager *res_manager, const char *str_storage, tarasque_entity *entity, allocator alloc);

void resource_manager_remove_supplicant(resource_manager *res_manager, tarasque_entity *entity, allocator alloc);


// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

#endif
