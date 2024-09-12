/**
 * @file resource.h
 * @author gabriel ()
 * @brief Manage resource files through this interface, serving the contained data and registering entity as using
 * storages to automate loading and unloading the resource files' contents.
 *
 * @version 0.1
 * @date 2024-04-09
 *
 * @copyright Copyright (c) 2024
 *
 */

#ifndef __BASILISK_RESOURCE_H__
#define __BASILISK_RESOURCE_H__

#include <basilisk.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* Opaque type to some data managing resources, their files and entities registered as using those. */
typedef struct resource_manager resource_manager;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* Allocates a resource manager object. */
resource_manager *resource_manager_create(allocator alloc);

/* Releases memory taken by a resource manager object, invalidating all resources that were queried. */
void resource_manager_destroy(resource_manager **res_manager, allocator alloc);

// -------------------------------------------------------------------------------------------------

/* Checks that a resource is accessible in a storage file, and creates a spot for it to be loaded and unloaded. */
bool resource_manager_touch(resource_manager *res_manager, const char *str_storage_path, const char *str_res_path, allocator alloc);

/* Tries to get a resource from a storage file and returns it. The resource needs to exist and its storage needs to have at least one supplicant (to be loaded). */
void *resource_manager_fetch(resource_manager *res_manager, const char *str_storage_path, const char *str_res_path, size_t *out_size);

/* Registers an entity as using a storage, adding it as a supplicant to the storage. If it is the first supplicant, the storage is loaded. */
void resource_manager_add_supplicant(resource_manager *res_manager, const char *str_storage_path, basilisk_entity *entity, allocator alloc);

/* Removes an entity as using a storage. If it was the last supplicant, the storage is unloaded. */
void resource_manager_remove_supplicant(resource_manager *res_manager, basilisk_entity *entity, allocator alloc);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

#endif
