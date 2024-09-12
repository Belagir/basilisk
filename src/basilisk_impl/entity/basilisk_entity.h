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

#include "../basilisk_common.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* Opaque type to a full entity containing user data and information about how it lives in the engine. */
typedef struct basilisk_engine_entity basilisk_engine_entity;
/* Quickhand for a range of entities. */
typedef RANGE(basilisk_engine_entity *) basilisk_engine_entity_range;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// LIFETIME

/* Creates an entity and returns a pointer to it. */
basilisk_engine_entity *basilisk_engine_entity_create(const identifier *id, basilisk_specific_entity user_data, basilisk_engine *handle, allocator alloc);
/* Destroys an entity and nullifies the pointer passed. */
void basilisk_engine_entity_destroy(basilisk_engine_entity **target, allocator alloc);

// -------------------------------------------------------------------------------------------------
// DIRECT GETTERS

/* Trusting that the entity points to the data of an entity, returns the full entity containing it. */
basilisk_engine_entity *basilisk_engine_entity_get_containing_full_entity(basilisk_entity *entity);
/* Returns a pointer to the specific user data in an entity, even if it was unallocated. */
basilisk_entity *basilisk_engine_entity_get_specific_data(basilisk_engine_entity *target);
/* Returns the engine instance hosting this entity. */
basilisk_engine *basilisk_engine_entity_get_host_engine_handle(basilisk_engine_entity *target);
/* Returns the name of an entity. */
const identifier *basilisk_engine_entity_get_name(const basilisk_engine_entity *target);

basilisk_engine_entity *basilisk_engine_entity_get_parent(basilisk_engine_entity *target);

bool basilisk_engine_entity_has_definition(basilisk_engine_entity *entity, basilisk_entity_definition entity_def);

// -------------------------------------------------------------------------------------------------
// HIERACHY MODIFICATIONS

/* Sets an entity to be a child of another. */
void basilisk_engine_entity_add_child(basilisk_engine_entity *target, basilisk_engine_entity *new_child, allocator alloc);
/* Removes the links between an entity and its eventual parent. */
void basilisk_engine_entity_deparent(basilisk_engine_entity *target);
/* Destroys all children of an entity, recursively. */
void basilisk_engine_entity_destroy_children(basilisk_engine_entity *target, allocator alloc);

// -------------------------------------------------------------------------------------------------
// CHILDREN SEARCHING

/* Finds a child of an entity. */
basilisk_engine_entity *basilisk_engine_entity_get_child(basilisk_engine_entity *target, const path *id);
/* Find a child that is directly under an entity. */
basilisk_engine_entity *basilisk_engine_entity_get_direct_child(basilisk_engine_entity *target, const identifier *id_path);
/* Returns an allocated range of all children of an entity, recursively. */
basilisk_engine_entity_range *basilisk_engine_entity_get_children(basilisk_engine_entity *target, allocator alloc);

// -------------------------------------------------------------------------------------------------
// CALLBACKS EXECUTION

/* Execute the on_frame() callback tied to an entity. */
void basilisk_engine_entity_step_frame(basilisk_engine_entity *target, f32 elapsed_ms);

/* Execute an event callback trusted to be linked to an entity. */
void basilisk_engine_entity_send_event(basilisk_engine_entity *target, basilisk_specific_event_subscription subscription_data, void *event_data);

/* Execute the on_init() callback tied to an entity */
void basilisk_engine_entity_init(basilisk_engine_entity *target);

/* Execute the on_deinit() callback tied to an entity */
void basilisk_engine_entity_deinit(basilisk_engine_entity *target);

#endif
