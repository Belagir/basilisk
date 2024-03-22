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

#include <tarasque.h>

#include "entity.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

typedef byte tarasque_entity_storage[];

/**
 * @brief Entity data structure aggregating user data with engine-related data.
 */
typedef struct tarasque_engine_entity {
    /** Name of the entity. */
    identifier *id;
    /** Non-owned reference to an eventual parent entity. */
    tarasque_engine_entity *parent;
    /** Array of all of the entity's children. */
    tarasque_entity_range *children;

    /** Entity callbacks to be used by the engine. */
    tarasque_specific_entity_callbacks callbacks;

    /** Engine owning the entity, used to redirect user's actions back to the whole engine. */
    tarasque_engine *host_handle;

    /** Size in bytes of the user's data. */
    size_t data_size;
    /** The user's data. */
    tarasque_entity_storage data;
} tarasque_engine_entity;

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
tarasque_engine_entity *tarasque_engine_entity_create(const identifier *id, tarasque_entity_specific_data_copy user_data, tarasque_engine *handle, allocator alloc)
{
    tarasque_engine_entity *new_entity = NULL;

    new_entity = alloc.malloc(alloc, sizeof(*new_entity) + user_data.data_size);

    if (new_entity) {
        *new_entity = (tarasque_engine_entity) {
                .id = range_create_dynamic_from_copy_of(alloc, RANGE_TO_ANY(id)),
                .parent = NULL,
                .children = range_create_dynamic(alloc, sizeof(*new_entity->children->data), TARASQUE_COLLECTIONS_START_LENGTH),

                .callbacks = user_data.callbacks,

                .host_handle = handle,

                .data_size = user_data.data_size,
        };

        bytewise_copy(new_entity->data, user_data.data, user_data.data_size);
    }

    return new_entity;
}

/**
 * @brief Destroys an entity by releasing its directly-owned memory, and nullifies the pointer passed to it.
 * Calling this function might leave children or a parent with dangling pointers : use with tarasque_engine_entity_deparent() and tarasque_engine_entity_destroy_children().
 *
 * @param[inout] target Entity to destroy.
 * @param[inout] alloc Allocator used to release memory.
 */
void tarasque_engine_entity_destroy(tarasque_engine_entity **target, allocator alloc)
{
    if (!target || !*target) {
        return;
    }

    range_destroy_dynamic(alloc, &RANGE_TO_ANY((*target)->children));
    range_destroy_dynamic(alloc, &RANGE_TO_ANY((*target)->id));

    alloc.free(alloc, *target);
    *target = NULL;
}

// -------------------------------------------------------------------------------------------------

/**
 * @brief Returns the full entity embedding the user data passed in argument.
 *
 * @param[in] entity user data trusted to be the data field of a tarasque_engine_entity.
 * @return tarasque_engine_entity *
 */
tarasque_engine_entity *tarasque_engine_entity_get_containing_full_entity(tarasque_entity *entity)
{
    if (!entity) {
        return NULL;
    }

    return CONTAINER_OF(entity, tarasque_engine_entity, data);
}

/**
 * @brief Returns a pointer to the user data field of an entity.
 *
 * @param[in] target Full entity.
 * @return tarasque_entity *
 */
tarasque_entity *tarasque_engine_entity_get_specific_data(tarasque_engine_entity *target)
{
    if (!target) {
        return NULL;
    }

    return target->data;
}

/**
 * @brief Returns a pointer to the engine instance containing the entity.
 *
 * @param[in] target Full entity.
 * @return tarasque_engine *
 */
tarasque_engine *tarasque_engine_entity_get_host_engine_handle(tarasque_engine_entity *target)
{
    if (!target) {
        return NULL;
    }

    return target->host_handle;
}

/**
 * @brief Returns a reference to the name of an entity.
 *
 * @param[in] target Target entity.
 * @return const identifier *
 */
const identifier *tarasque_engine_entity_get_name(const tarasque_engine_entity *target)
{
    if (!target) {
        return NULL;
    }

    return target->id;
}

/**
 * @brief
 *
 * @param target
 * @return
 */
tarasque_engine_entity *tarasque_engine_entity_get_parent(tarasque_engine_entity *target)
{
    if (!target) {
        return NULL;
    }

    return target->parent;
}

// -------------------------------------------------------------------------------------------------

/**
 * @brief Add a child to another entity, inserting it into the children array and modifying the parent pointer of the new child entity.
 *
 * @param[inout] target Parent entity receiving the child.
 * @param[inout] new_child Entity added as child.
 * @param[inout] alloc Allocator used for the children array insertion.
 */
void tarasque_engine_entity_add_child(tarasque_engine_entity *target, tarasque_engine_entity *new_child, allocator alloc)
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
void tarasque_engine_entity_deparent(tarasque_engine_entity *target)
{
    if (!target || !target->parent) {
        return;
    }

    (void) sorted_range_remove_from(RANGE_TO_ANY(target->parent->children), &identifier_compare_doubleref, &target);
    target->parent = NULL;
}

/**
 * @brief Destroys all children of an entity, from the deepest entity to the direct children of the entity.
 * Each child entity is destroyed before its parent.
 *
 * @param[inout] target Entity the children are destroyed from.
 * @param[inout] alloc Allocator used to release the memory of the children entities.
 */
void tarasque_engine_entity_destroy_children(tarasque_engine_entity *target, allocator alloc)
{
    tarasque_entity_range *all_children = NULL;

    if (!target) {
        return;
    }

    all_children = tarasque_engine_entity_get_children(target, alloc);
    for (int i = (int) all_children->length - 1 ; i >= 0 ; i--) {
        tarasque_engine_entity_destroy(all_children->data + i, alloc);
    }
    range_destroy_dynamic(alloc, &RANGE_TO_ANY(all_children));

    target->children->length = 0u;
}

/**
 * @brief Get a single child from an entity by its path from the entity.
 *
 * @param[inout] target Supposed parent of the searched entity.
 * @param[inout] id_path path of indetifiers leading to the searched entity.
 * @return entity*
 */
tarasque_engine_entity *tarasque_engine_entity_get_child(tarasque_engine_entity *target, const path *id_path)
{
    tarasque_engine_entity *visited_entity = NULL;
    size_t pos_path = 0u;

    if (!target) {
        return NULL;
    } else if (!id_path || (id_path->length == 0u)) {
        return target;
    }

    visited_entity = target;
    while (visited_entity && (pos_path < id_path->length)) {
        visited_entity = tarasque_engine_entity_get_direct_child(visited_entity, id_path->data[pos_path]);
        pos_path += (size_t) (visited_entity != NULL);
    }

    return visited_entity;
}

/**
 * @brief Returns a direct child of an entity, by name, if it exists.
 *
 * @param[in] target Target parent entity.
 * @param[in] id_path Name of the searched child entity.
 * @return entity *
 */
tarasque_engine_entity *tarasque_engine_entity_get_direct_child(tarasque_engine_entity *target, const identifier *id)
{
    bool found_child = false;
    size_t pos_child = 0u;

    if (!target || !id) {
        return NULL;
    }

    found_child = sorted_range_find_in(
            RANGE_TO_ANY(target->children),
            &identifier_compare_doubleref,
            &(const identifier **) { &id },
            &pos_child);

    if (found_child) {
        return target->children->data[pos_child];
    }
    return NULL;
}

/**
 * @brief Returns an allocated range of ALL children of an entity.
 *
 * @param[in] target Entity from which to extract children.
 * @param[inout] alloc Allocator used to create the returned range.
 * @return tarasque_entity_range*
 */
tarasque_entity_range *tarasque_engine_entity_get_children(tarasque_engine_entity *target, allocator alloc)
{
    size_t child_pos = 0u;
    tarasque_entity_range *entities = NULL;

    if (!target) {
        return NULL;
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
void tarasque_engine_entity_step_frame(tarasque_engine_entity *target, f32 elapsed_ms)
{
    if (!target) {
        return;
    }
    if (target->callbacks.on_frame) {
        target->callbacks.on_frame(target->data, elapsed_ms);
    }
}

/**
 * @brief Calls an arbitrary event callback over an entity.
 *
 * @param[inout] target Target entity.
 * @param[in] callback Event callback.
 * @param[inout] event_data Event data passed to the callback.
 */
void tarasque_engine_entity_send_event(tarasque_engine_entity *target, tarasque_specific_event_subscription subscription_data, void *event_data)
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
void tarasque_engine_entity_init(tarasque_engine_entity *target)
{
    if (!target) {
        return;
    }

    if (target->callbacks.on_init) {
        target->callbacks.on_init(target->data);
    }
}

/**
 * @brief Calls the `.on_deinit()` callback of some entity, if it exists.
 *
 * @param[inout] target Target entity.
 */
void tarasque_engine_entity_deinit(tarasque_engine_entity *target)
{
    if (!target) {
        return;
    }

    if (target->callbacks.on_deinit) {
        target->callbacks.on_deinit(target->data);
    }
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Creates an allocated copy of some user data.
 *
 * @param[in] user_data Copied user data.
 * @param[inout] alloc Allocator used for the copy.
 * @return
 */
tarasque_entity_specific_data_copy tarasque_entity_specific_data_copy_create(tarasque_specific_entity user_data, allocator alloc)
{
    tarasque_entity_specific_data_copy copy = {
            .callbacks = {
                    .on_init   = user_data.callbacks.on_init,
                    .on_deinit = user_data.callbacks.on_deinit,
                    .on_frame  = user_data.callbacks.on_frame,
            }
    };

    if (user_data.data_size > 0u) {
        copy.data = alloc.malloc(alloc, user_data.data_size);
        copy.data_size = user_data.data_size;
    }

    if (user_data.data) {
        bytewise_copy(copy.data, user_data.data, user_data.data_size);
    }

    return copy;
}

/**
 * @brief Destroys a copy of user data that was previously allocated and zero-out its contents.
 *
 * @param[inout] user_data copy of user data.
 * @param[inout] alloc Allocator used to release the object.
 */
void tarasque_entity_specific_data_copy_destroy(tarasque_entity_specific_data_copy *user_data, allocator alloc)
{
    if (!user_data) {
        return;
    }

    if (user_data->data) {
        alloc.free(alloc, user_data->data);
    }

    *user_data = (tarasque_entity_specific_data_copy) { 0u };
}
