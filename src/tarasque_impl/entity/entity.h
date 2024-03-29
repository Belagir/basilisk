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

/* Opaque type to a full entity containing user data and information about how it lives in the engine. */
typedef struct tarasque_engine_entity tarasque_engine_entity;
/* Quickhand for a range of entities. */
typedef RANGE(tarasque_engine_entity *) tarasque_engine_entity_range;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// LIFETIME

/* Creates an entity and returns a pointer to it. */
tarasque_engine_entity *tarasque_engine_entity_create(const identifier *id, tarasque_specific_entity user_data, tarasque_engine *handle, allocator alloc);
/* Destroys an entity and nullifies the pointer passed. */
void tarasque_engine_entity_destroy(tarasque_engine_entity **target, allocator alloc);

// -------------------------------------------------------------------------------------------------
// DIRECT GETTERS

/* Trusting that the entity points to the data of an entity, returns the full entity containing it. */
tarasque_engine_entity *tarasque_engine_entity_get_containing_full_entity(tarasque_entity *entity);
/* Returns a pointer to the specific user data in an entity, even if it was unallocated. */
tarasque_entity *tarasque_engine_entity_get_specific_data(tarasque_engine_entity *target);
/* Returns the engine instance hosting this entity. */
tarasque_engine *tarasque_engine_entity_get_host_engine_handle(tarasque_engine_entity *target);
/* Returns the name of an entity. */
const identifier *tarasque_engine_entity_get_name(const tarasque_engine_entity *target);

tarasque_engine_entity *tarasque_engine_entity_get_parent(tarasque_engine_entity *target);

bool tarasque_engine_entity_has_definition(tarasque_engine_entity *entity, tarasque_entity_definition entity_def);

// -------------------------------------------------------------------------------------------------
// HIERACHY MODIFICATIONS

/* Sets an entity to be a child of another. */
void tarasque_engine_entity_add_child(tarasque_engine_entity *target, tarasque_engine_entity *new_child, allocator alloc);
/* Removes the links between an entity and its eventual parent. */
void tarasque_engine_entity_deparent(tarasque_engine_entity *target);
/* Destroys all children of an entity, recursively. */
void tarasque_engine_entity_destroy_children(tarasque_engine_entity *target, allocator alloc);

// -------------------------------------------------------------------------------------------------
// CHILDREN SEARCHING

/* Finds a child of an entity. */
tarasque_engine_entity *tarasque_engine_entity_get_child(tarasque_engine_entity *target, const path *id);
/* Find a child that is directly under an entity. */
tarasque_engine_entity *tarasque_engine_entity_get_direct_child(tarasque_engine_entity *target, const identifier *id_path);
/* Returns an allocated range of all children of an entity, recursively. */
tarasque_engine_entity_range *tarasque_engine_entity_get_children(tarasque_engine_entity *target, allocator alloc);

// -------------------------------------------------------------------------------------------------
// CALLBACKS EXECUTION

/* Execute the on_frame() callback tied to an entity. */
void tarasque_engine_entity_step_frame(tarasque_engine_entity *target, f32 elapsed_ms);

/* Execute an event callback trusted to be linked to an entity. */
void tarasque_engine_entity_send_event(tarasque_engine_entity *target, tarasque_specific_event_subscription subscription_data, void *event_data);

/* Execute the on_init() callback tied to an entity */
void tarasque_engine_entity_init(tarasque_engine_entity *target);

/* Execute the on_deinit() callback tied to an entity */
void tarasque_engine_entity_deinit(tarasque_engine_entity *target);

#endif
