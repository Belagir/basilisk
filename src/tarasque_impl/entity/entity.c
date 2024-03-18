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

#include "entity.h"
#include "../engine/tarasque_impl.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Entity data structure aggregating user data with engine-related data.
 */
typedef struct entity {
    /** Name of the entity. */
    identifier *id;
    /** Non-owned reference to an eventual parent entity. */
    entity *parent;
    /** Array of all of the entity's children. */
    entity_range *children;

    /** Copy of the user-specified data. */
    entity_user_data_copy user_data;
} entity;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Creates a newly allocated entity and returns a pointer to it.
 *
 * @param[in] id Name (copied) of the new entity.
 * @param[in] user_data Copy (copied again) of some user data.
 * @param[inout] handle Handle to the engine instance to allow the created entity to.
 * @param[inout] alloc Allocator used for the creation of the entity.
 * @return entity *
 */
entity *entity_create(const identifier *id, entity_user_data_copy user_data, allocator alloc)
{
    entity *new_entity = NULL;

    new_entity = alloc.malloc(alloc, sizeof(*new_entity));

    if (new_entity) {
        *new_entity = (entity) {
                .id = range_create_dynamic_from_copy_of(alloc, range_to_any(id)),
                .parent = NULL,
                .children = range_create_dynamic(alloc, sizeof(*new_entity->children->data), TARASQUE_COLLECTIONS_START_LENGTH),
                .user_data = entity_user_data_copy_create(user_data, alloc),
        };
    }

    return new_entity;
}

/**
 * @brief Destroys an entity by releasing its directly-owned memory, and nullifies the pointer passed to it.
 * Calling this function might leave children or a parent with dangling pointers : use with entity_deparent() and entity_destroy_children().
 *
 * @param[inout] target Entity to destroy.
 * @param[inout] alloc Allocator used to release memory.
 */
void entity_destroy(entity **target, allocator alloc)
{
    if (!target || !*target) {
        return;
    }

    range_destroy_dynamic(alloc, &range_to_any((*target)->children));
    range_destroy_dynamic(alloc, &range_to_any((*target)->id));
    entity_user_data_copy_destroy(&(*target)->user_data, alloc);

    alloc.free(alloc, *target);
    *target = NULL;
}

// -------------------------------------------------------------------------------------------------

/**
 * @brief Add a child to another entity, inserting it into the children array and modifying the parent pointer of the new child entity.
 *
 * @param[inout] target Parent entity receiving the child.
 * @param[inout] new_child Entity added as child.
 * @param[inout] alloc Allocator used for the children array insertion.
 */
void entity_add_child(entity *target, entity *new_child, allocator alloc)
{
    if (!target || !new_child) {
        return;
    }

    target->children = range_ensure_capacity(alloc, range_to_any(target->children), 1);
    (void) sorted_range_insert_in(range_to_any(target->children), &identifier_compare_doubleref, &new_child);
    new_child->parent = target;
}

/**
 * @brief Removes the parent-children relationship between an entity and its eventual parent.
 * If the entity has a parent, the entity is removed from the parent's children array and the praent entity pointer is nullified.
 *
 * @param[inout] target Entity to de-parent.
 */
void entity_deparent(entity *target)
{
    if (!target || !target->parent) {
        return;
    }

    (void) sorted_range_remove_from(range_to_any(target->parent->children), &identifier_compare_doubleref, &target);
    target->parent = NULL;
}

/**
 * @brief Destroys all children of an entity, from the deepest entity to the direct children of the entity.
 * Each child entity is destroyed before its parent.
 *
 * @param[inout] target Entity the children are destroyed from.
 * @param[inout] alloc Allocator used to release the memory of the children entities.
 */
void entity_destroy_children(entity *target, allocator alloc)
{
    entity_range *all_children = NULL;

    if (!target) {
        return;
    }

    all_children = entity_get_children(target, alloc);
    for (int i = (int) all_children->length - 1 ; i >= 0 ; i--) {
        entity_destroy(all_children->data + i, alloc);
    }
    range_destroy_dynamic(alloc, &range_to_any(all_children));

    target->children->length = 0u;
}

/**
 * @brief Get a single child from an entity by its path from the entity.
 *
 * @param[inout] target Supposed parent of the searched entity.
 * @param[inout] id_path path of indetifiers leading to the searched entity.
 * @return entity*
 */
entity *entity_get_child(entity *target, const path *id_path)
{
    entity *visited_entity = NULL;
    size_t pos_path = 0u;

    if (!target) {
        return NULL;
    } else if (!id_path || (id_path->length == 0u)) {
        return target;
    }

    visited_entity = target;
    while (visited_entity && (pos_path < id_path->length)) {
        visited_entity = entity_get_direct_child(visited_entity, id_path->data[pos_path]);
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
entity *entity_get_direct_child(entity *target, const identifier *id)
{
    bool found_child = false;
    size_t pos_child = 0u;

    if (!target || !id) {
        return NULL;
    }

    found_child = sorted_range_find_in(
            range_to_any(target->children),
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
 * @return entity_range*
 */
entity_range *entity_get_children(entity *target, allocator alloc)
{
    size_t child_pos = 0u;
    entity_range *entities = NULL;

    if (!target) {
        return NULL;
    }

    entities = range_create_dynamic_from_copy_of(alloc, range_to_any(target->children));

    while (child_pos < entities->length) {
        entities = range_ensure_capacity(alloc, range_to_any(entities), entities->data[child_pos]->children->length);
        range_insert_range(range_to_any(entities), entities->length, range_to_any(entities->data[child_pos]->children));
        child_pos += 1u;
    }

    return entities;
}

/**
 * @brief Returns a reference to the name of an entity.
 *
 * @param[in] target Target entity.
 * @return const identifier *
 */
const identifier *entity_get_name(const entity *target)
{
    if (!target) {
        return NULL;
    }

    return target->id;
}

/**
 * @brief Calls the `.on_frame()` callback of some entity, if it exists.
 *
 * @param[inout] target Target entity.
 * @param[in] elapsed_ms Number of elapsed milliseconds, hopefully since the last time the callback was called.
 * @param[inout] handle Engine handle so the user code can modify the state of the engine.
 */
void entity_step_frame(entity *target, f32 elapsed_ms, tarasque_engine *handle)
{
    if (!target) {
        return;
    }

    if (target->user_data.on_frame) {
        target->user_data.on_frame(target->user_data.data, elapsed_ms, &(tarasque_entity_scene) { handle, target });
    }
}

/**
 * @brief Calls an arbitrary event callback over an entity.
 *
 * @param[inout] target Target entity.
 * @param[in] callback Event callback.
 * @param[inout] event_data Event data passed to the callback.
 */
void entity_send_event(entity *target, void (*callback)(void *entity_data, void *event_data), void *event_data)
{
    if (!target || !callback) {
        return;
    }

    callback(target->user_data.data, event_data);
}

/**
 * @brief Calls the `.on_init()` callback of some entity, if it exists.
 *
 * @param[inout] target Target entity.
 * @param[in] handle Engine handle so the user code can modify the state of the engine.
 */
void entity_init(entity *target, tarasque_engine *handle)
{
    if (!target) {
        return;
    }

    if (target->user_data.on_init) {
        target->user_data.on_init(target->user_data.data, &(tarasque_entity_scene) { handle, target });
    }
}

/**
 * @brief Calls the `.on_deinit()` callback of some entity, if it exists.
 *
 * @param[inout] target Target entity.
 * @param[in] handle Engine handle so the user code can modify the state of the engine.
 */
void entity_deinit(entity *target, tarasque_engine *handle)
{
    if (!target) {
        return;
    }

    if (target->user_data.on_deinit) {
        target->user_data.on_deinit(target->user_data.data, &(tarasque_entity_scene) { handle, target });
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
entity_user_data_copy entity_user_data_copy_create(entity_user_data user_data, allocator alloc)
{
    entity_user_data_copy copy = {
            .on_init = user_data.on_init,
            .on_deinit = user_data.on_deinit,
            .on_frame = user_data.on_frame,
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
void entity_user_data_copy_destroy(entity_user_data_copy *user_data, allocator alloc)
{
    if (!user_data) {
        return;
    }

    if (user_data->data) {
        alloc.free(alloc, user_data->data);
    }

    *user_data = (entity_user_data_copy) { 0u };
}
