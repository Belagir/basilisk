/**
 * @file entity.h
 * @author gabriel ()
 * @brief Create and manipulate entities with user-defined behavior and troubled family relations.
 *
 * Entities are defined by some core data the user passed, a name, an eventual parent entity and any number of children.
 *
 * @version 0.1
 * @date 2024-03-07
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "../common.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* Opaque type to some entity. */
typedef struct entity entity;

/* Quickhand for a range of entities. */
typedef range(entity *) entity_range;

/* Redefinition of the entity_user_data type to signal memory allocation in opposition of the user-managed entity_user_data variables. */
typedef entity_user_data entity_user_data_copy;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* Creates an entity and returns a pointer to it. */
entity *entity_create(const identifier *id, entity_user_data_copy user_data, allocator alloc);
/* Destroys an entity and nullifies the pointer passed. */
void entity_destroy(entity **target, allocator alloc);

// -------------------------------------------------------------------------------------------------

/* Sets an entity to be a child of another. */
void entity_add_child(entity *target, entity *new_child, allocator alloc);
/* Removes the links between an entity and its eventual parent. */
void entity_deparent(entity *target);
/* Destroys all children of an entity, recursively. */
void entity_destroy_children(entity *target, allocator alloc);

// -------------------------------------------------------------------------------------------------

/* Finds a child of an entity. */
entity *entity_get_child(entity *target, const path *id_path);
/* Returns an allocated range of all children of an entity, recursively. */
entity_range *entity_get_children(entity *target, allocator alloc);

/* Returns the name of an entity. */
const identifier *entity_get_name(const entity *target);

// -------------------------------------------------------------------------------------------------

/* Execute the on_frame() callback tied to an entity. */
void entity_step_frame(entity *target, f32 elapsed_ms, tarasque_engine *handle);

/* Execute an event callback trusted to be linked to an entity. */
void entity_send_event(entity *target, void (*callback)(void *entity_data, void *event_data), void *event_data);

/* Execute the on_init() callback tied to an entity */
void entity_init(entity *target, tarasque_engine *handle);

/* Execute the on_deinit() callback tied to an entity */
void entity_deinit(entity *target, tarasque_engine *handle);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* Copy entity user data on the heap, and returns a pointer to it. */
entity_user_data_copy entity_user_data_copy_create(entity_user_data user_data, allocator alloc);
/* Destroys some entity user data copy and nullifies the passed pointer. */
void entity_user_data_copy_destroy(entity_user_data_copy *user_data, allocator alloc);

#endif
