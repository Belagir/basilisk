/**
 * @file entity.c
 * @author gabriel ()
 * @brief Implementation file for everything related to entities.
 * @version 0.1
 * @date 2024-03-07
 *
 * @copyright Copyright (c) 2024
 *
 */
#include <ustd/sorting.h>
#include <stdio.h>

#include <basilisk.h>

#include "basilisk_entity.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @typedef basilisk_entity_storage
 * @brief Shorthand typedef for a byte array with a more explicit syntax.
 */
typedef byte basilisk_entity_storage[];

/**
 * @brief Entity data structure aggregating user data with engine-related data.
 */
typedef struct basilisk_engine_entity {
    /** Name of the entity. */
    identifier *id;
    /** Non-owned reference to an eventual parent entity. */
    basilisk_engine_entity *parent;
    /** Array of all of the entity's children. */
    basilisk_engine_entity_range *children;
    /** Engine owning the entity, used to redirect user's actions back to the whole engine. */
    basilisk_engine *host_handle;

    basilisk_entity_definition self_definition;

    /** The user's data. */
    basilisk_entity_storage data;
} basilisk_engine_entity;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static bool basilisk_entity_definition_unit_is_same_as(basilisk_entity_definition def_unit, basilisk_entity_definition broad_def);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Creates a newly allocated entity and returns a pointer to it.
 *
 * @param[in] id Name (copied) of the new entity.
 * @param[in] user_data Copy (copied again) of some user data.
 * @param[inout] handle Handle to the engine instance to allow the created entity to change its state.
 * @param[inout] alloc Allocator used for the creation of the entity.
 * @return entity *
 */
basilisk_engine_entity *basilisk_engine_entity_create(const identifier *id, basilisk_specific_entity user_data, basilisk_engine *handle, allocator alloc)
{
    basilisk_engine_entity *new_entity = nullptr;
    const basilisk_entity_definition *subtyped_definition = nullptr;

    new_entity = alloc.malloc(alloc, sizeof(*new_entity) + user_data.entity_def.data_size);

    if (new_entity) {
        // core informations
        *new_entity = (basilisk_engine_entity) {
                .id = range_create_dynamic_from_copy_of(alloc, RANGE_TO_ANY(id)),
                .parent = nullptr,
                .children = range_create_dynamic(alloc, sizeof(*new_entity->children->data), BASILISK_COLLECTIONS_START_LENGTH),
                .host_handle = handle,

                .self_definition = {
                        .on_init = user_data.entity_def.on_init,
                        .on_deinit = user_data.entity_def.on_deinit,
                        .on_frame = user_data.entity_def.on_frame,

                        .data_size = user_data.entity_def.data_size,
                }
        };

        // optional starting data
        if (user_data.data) {
            bytewise_copy(new_entity->data, user_data.data, user_data.entity_def.data_size);
        }
    }

    return new_entity;
}

/**
 * @brief Destroys an entity by releasing its directly-owned memory, and nullifies the pointer passed to it.
 * Calling this function might leave children or a parent with dangling pointers : use with basilisk_engine_entity_deparent() and basilisk_engine_entity_destroy_children().
 *
 * @param[inout] target Entity to destroy.
 * @param[inout] alloc Allocator used to release memory.
 */
void basilisk_engine_entity_destroy(basilisk_engine_entity **target, allocator alloc)
{
    if (!target || !*target) {
        return;
    }

    range_destroy_dynamic(alloc, &RANGE_TO_ANY((*target)->children));
    range_destroy_dynamic(alloc, &RANGE_TO_ANY((*target)->id));

    alloc.free(alloc, *target);
    *target = nullptr;
}

// -------------------------------------------------------------------------------------------------

/**
 * @brief Returns the full entity embedding the user data passed in argument.
 *
 * @param[in] entity user data trusted to be the data field of a basilisk_engine_entity.
 * @return basilisk_engine_entity *
 */
basilisk_engine_entity *basilisk_engine_entity_get_containing_full_entity(const basilisk_entity *entity)
{
    if (!entity) {
        return nullptr;
    }

    return CONTAINER_OF(entity, basilisk_engine_entity, data);
}

/**
 * @brief Returns a pointer to the user data field of an entity.
 *
 * @param[in] target Full entity.
 * @return basilisk_entity *
 */
basilisk_entity *basilisk_engine_entity_get_specific_data(basilisk_engine_entity *target)
{
    if (!target) {
        return nullptr;
    }

    return target->data;
}

/**
 * @brief Returns a pointer to the engine instance containing the entity.
 *
 * @param[in] target Full entity.
 * @return basilisk_engine *
 */
basilisk_engine *basilisk_engine_entity_get_host_engine_handle(basilisk_engine_entity *target)
{
    if (!target) {
        return nullptr;
    }

    return target->host_handle;
}

/**
 * @brief Returns a reference to the name of an entity.
 *
 * @param[in] target Target entity.
 * @return const identifier *
 */
const identifier *basilisk_engine_entity_get_name(const basilisk_engine_entity *target)
{
    if (!target) {
        return nullptr;
    }

    return target->id;
}

/**
 * @brief Returns the direct parent of the entity.
 *
 * @param[in] target Target entity.
 * @return basilisk_engine_entity *
 */
basilisk_engine_entity *basilisk_engine_entity_get_parent(basilisk_engine_entity *target)
{
    if (!target) {
        return nullptr;
    }

    return target->parent;
}

/**
 * @brief
 *
 * @param entity
 * @param entity_def
 * @return
 */
bool basilisk_engine_entity_has_definition(const basilisk_engine_entity *entity, basilisk_entity_definition entity_def)
{
    bool has_def = false;
    size_t pos = 0u;

    if (!entity) {
        return false;
    }

    has_def = basilisk_entity_definition_unit_is_same_as(entity->self_definition, entity_def);

    return has_def;
}

// -------------------------------------------------------------------------------------------------

/**
 * @brief Add a child to another entity, inserting it into the children array and modifying the parent pointer of the new child entity.
 *
 * @param[inout] target Parent entity receiving the child.
 * @param[inout] new_child Entity added as child.
 * @param[inout] alloc Allocator used for the children array insertion.
 */
void basilisk_engine_entity_add_child(basilisk_engine_entity *target, basilisk_engine_entity *new_child, allocator alloc)
{
    if (!target || !new_child) {
        return;
    }

    target->children = range_ensure_capacity(alloc, RANGE_TO_ANY(target->children), 1);
    (void) sorted_range_insert_in(RANGE_TO_ANY(target->children), &identifier_compare_doubleref, &new_child);
    new_child->parent = target;
}

/**
 * @brief Removes the parent-children relationship between an entity and its eventual parent.
 * If the entity has a parent, the entity is removed from the parent's children array and the praent entity pointer is nullified.
 *
 * @param[inout] target Entity to de-parent.
 */
void basilisk_engine_entity_deparent(basilisk_engine_entity *target)
{
    if (!target || !target->parent) {
        return;
    }

    (void) sorted_range_remove_from(RANGE_TO_ANY(target->parent->children), &identifier_compare_doubleref, &target);
    target->parent = nullptr;
}

/**
 * @brief Destroys all children of an entity, from the deepest entity to the direct children of the entity.
 * Each child entity is destroyed before its parent.
 *
 * @param[inout] target Entity the children are destroyed from.
 * @param[inout] alloc Allocator used to release the memory of the children entities.
 */
void basilisk_engine_entity_destroy_children(basilisk_engine_entity *target, allocator alloc)
{
    basilisk_engine_entity_range *all_children = nullptr;

    if (!target) {
        return;
    }

    all_children = basilisk_engine_entity_get_children(target, alloc);
    for (int i = (int) all_children->length - 1 ; i >= 0 ; i--) {
        basilisk_engine_entity_destroy(all_children->data + i, alloc);
    }
    range_destroy_dynamic(alloc, &RANGE_TO_ANY(all_children));

    target->children->length = 0u;
}

/**
 * @brief Get a single child from an entity by its path from the entity.
 *
 * @param[inout] target Supposed parent of the searched entity.
 * @param[inout] id_path path of indetifiers leading to the searched entity.
 * @return basilisk_engine_entity*
 */
basilisk_engine_entity *basilisk_engine_entity_get_child(basilisk_engine_entity *target, const path *id_path)
{
    basilisk_engine_entity *visited_entity = nullptr;
    size_t pos_path = 0u;

    if (!target) {
        return nullptr;
    } else if (!id_path || (id_path->length == 0u)) {
        return target;
    }

    visited_entity = target;
    while (visited_entity && (pos_path < id_path->length)) {
        visited_entity = basilisk_engine_entity_get_direct_child(visited_entity, id_path->data[pos_path]);
        pos_path += (size_t) (visited_entity != nullptr);
    }

    return visited_entity;
}

/**
 * @brief Returns a direct child of an entity, by name, if it exists.
 *
 * @param[in] target Target parent entity.
 * @param[in] id_path Name of the searched child entity.
 * @return basilisk_engine_entity *
 */
basilisk_engine_entity *basilisk_engine_entity_get_direct_child(basilisk_engine_entity *target, const identifier *id)
{
    bool found_child = false;
    size_t pos_child = 0u;

    if (!target || !id) {
        return nullptr;
    }

    found_child = sorted_range_find_in(
            RANGE_TO_ANY(target->children),
            &identifier_compare_doubleref,
            &(const identifier **) { &id },
            &pos_child);

    if (found_child) {
        return target->children->data[pos_child];
    }
    return nullptr;
}

/**
 * @brief Returns an allocated range of ALL children of an entity.
 *
 * @param[in] target Entity from which to extract children.
 * @param[inout] alloc Allocator used to create the returned range.
 * @return basilisk_engine_entity_range*
 */
basilisk_engine_entity_range *basilisk_engine_entity_get_children(basilisk_engine_entity *target, allocator alloc)
{
    size_t child_pos = 0u;
    basilisk_engine_entity_range *entities = nullptr;

    if (!target) {
        return nullptr;
    }

    entities = range_create_dynamic_from_copy_of(alloc, RANGE_TO_ANY(target->children));

    while (child_pos < entities->length) {
        entities = range_ensure_capacity(alloc, RANGE_TO_ANY(entities), entities->data[child_pos]->children->length);
        range_insert_range(RANGE_TO_ANY(entities), entities->length, RANGE_TO_ANY(entities->data[child_pos]->children));
        child_pos += 1u;
    }

    return entities;
}

/**
 * @brief Calls the `.on_frame()` callback of some entity, if it exists.
 *
 * @param[inout] target Target entity.
 * @param[in] elapsed_ms Number of elapsed milliseconds, hopefully since the last time the callback was called.
 */
void basilisk_engine_entity_step_frame(basilisk_engine_entity *target, f32 elapsed_ms)
{
    if (!target) {
        return;
    }

    if (target->self_definition.on_frame) {
        target->self_definition.on_frame(target->data, elapsed_ms);
    }
}

/**
 * @brief Calls an arbitrary event callback over an entity.
 *
 * @param[inout] target Target entity.
 * @param[in] callback Event callback.
 * @param[inout] event_data Event data passed to the callback.
 */
void basilisk_engine_entity_send_event(basilisk_engine_entity *target, basilisk_specific_event_subscription subscription_data, void *event_data)
{
    if (!target || !subscription_data.callback) {
        return;
    }

    subscription_data.callback(target->data, event_data);
}

/**
 * @brief Calls the `.on_init()` callback of some entity, if it exists.
 *
 * @param[inout] target Target entity.
 */
void basilisk_engine_entity_init(basilisk_engine_entity *target)
{
    if (!target) {
        return;
    }

    if (target->self_definition.on_init) {
        target->self_definition.on_init(target->data);
    }
}

/**
 * @brief Calls the `.on_deinit()` callback of some entity, if it exists.
 *
 * @param[inout] target Target entity.
 */
void basilisk_engine_entity_deinit(basilisk_engine_entity *target)
{
    if (!target) {
        return;
    }

    if (target->self_definition.on_deinit) {
        target->self_definition.on_deinit(target->data);
    }
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static bool basilisk_entity_definition_unit_is_same_as(basilisk_entity_definition def_unit, basilisk_entity_definition broad_def)
{
    return   (def_unit.data_size == broad_def.data_size)
            && (def_unit.on_init   == broad_def.on_init)
            && (def_unit.on_frame  == broad_def.on_frame)
            && (def_unit.on_deinit == broad_def.on_deinit);
}
