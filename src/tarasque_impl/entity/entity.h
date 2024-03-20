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

typedef struct tarasque_entity tarasque_entity;
/* Quickhand for a range of entities. */
typedef range(tarasque_entity *) tarasque_entity_range;

/**
 * @brief Entity data structure aggregating user data with engine-related data.
 */
typedef struct tarasque_entity {
    /** Name of the entity. */
    identifier *id;
    /** Non-owned reference to an eventual parent entity. */
    tarasque_entity *parent;
    /** Array of all of the entity's children. */
    tarasque_entity_range *children;

    entity_callbacks callbacks;

    tarasque_engine *host_handle;

    size_t data_size;
    byte data[];
} tarasque_entity;


/* Redefinition of the entity_user_data type to signal memory allocation in opposition of the user-managed entity_user_data variables. */
typedef entity_user_data entity_user_data_copy;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* Creates an entity and returns a pointer to it. */
tarasque_entity *entity_create(const identifier *id, entity_user_data_copy user_data, tarasque_engine *handle, allocator alloc);
/* Destroys an entity and nullifies the pointer passed. */
void entity_destroy(tarasque_entity **target, allocator alloc);

// -------------------------------------------------------------------------------------------------

/* Sets an entity to be a child of another. */
void entity_add_child(tarasque_entity *target, tarasque_entity *new_child, allocator alloc);
/* Removes the links between an entity and its eventual parent. */
void entity_deparent(tarasque_entity *target);
/* Destroys all children of an entity, recursively. */
void entity_destroy_children(tarasque_entity *target, allocator alloc);

// -------------------------------------------------------------------------------------------------

/* Finds a child of an entity. */
tarasque_entity *entity_get_child(tarasque_entity *target, const path *id);
/* Find a child that is directly under an entity. */
tarasque_entity *entity_get_direct_child(tarasque_entity *target, const identifier *id_path);


/* Returns an allocated range of all children of an entity, recursively. */
tarasque_entity_range *entity_get_children(tarasque_entity *target, allocator alloc);

/* Returns the name of an entity. */
const identifier *entity_get_name(const tarasque_entity *target);

// -------------------------------------------------------------------------------------------------

/* Execute the on_frame() callback tied to an entity. */
void entity_step_frame(tarasque_entity *target, f32 elapsed_ms, tarasque_engine *handle);

/* Execute an event callback trusted to be linked to an entity. */
void entity_send_event(tarasque_entity *target, event_subscription_user_data subscription_data, void *event_data, tarasque_engine *handle);

/* Execute the on_init() callback tied to an entity */
void entity_init(tarasque_entity *target, tarasque_engine *handle);

/* Execute the on_deinit() callback tied to an entity */
void entity_deinit(tarasque_entity *target, tarasque_engine *handle);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* Copy entity user data on the heap, and returns a pointer to it. */
entity_user_data_copy entity_user_data_copy_create(entity_user_data user_data, allocator alloc);
/* Destroys some entity user data copy and nullifies the passed pointer. */
void entity_user_data_copy_destroy(entity_user_data_copy *user_data, allocator alloc);

#endif
