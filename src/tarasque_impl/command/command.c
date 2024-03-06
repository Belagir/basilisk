/**
 * @file command.c
 * @author gabriel
 * @brief Implementation file for all that is related to commands sent to the engine.
 * @version 0.1
 * @date 2024-03-06
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "command.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Command queue data details.
 */
typedef struct command_queue {
    /** Range of commands. */
    range(command) *queue_impl;
} command_queue;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Creates a command to add an entity in the game tree later.
 *
 * @param[in] source Entity that sent the command.
 * @param[in] id_path Zero-terminated string (copied) representing a path of entities names separated by '/'.
 * @param[in] id Zero-terminated string (copied) representing the name of the new entity.
 * @param[in] template User-defined data for the entity.
 * @param[inout] alloc Allocator used for the allocation of the command.
 * @return a fresh command to be queued.
 */
command command_create_add_entity(entity *source, const char *id_path, const char *id, entity_core template, allocator alloc)
{
    command new_cmd = { 0u };

    if (!id_path || !id) {
        return (command) { .flavor = COMMAND_INVALID };
    }

    new_cmd = (command) {
            .flavor = COMMAND_ADD_ENTITY,
            .source = source,
            .specific.add_entity = {
                    .id = identifier_from_cstring(id, alloc),
                    .id_path = path_from_cstring(id_path, alloc),
                    .template = entity_core_copy_create(template, alloc),
             },
    };

    return new_cmd;
}

/**
 * @brief Creates a command to remove an entity from the game tree later.
 *
 * @param[in] source Entity that sent the command.
 * @param[in] id_path Zero-terminated string (copied) representing a path of entities names separated by '/'.
 * @param[inout] alloc Allocator used for the allocation of the command.
 * @return A fresh command to be queued.
 */
command command_create_remove_entity(entity *source, const char *id_path, allocator alloc)
{
    command new_cmd = { 0u };

    if (!id_path) {
        return (command) { .flavor = COMMAND_INVALID };
    }

    new_cmd = (command) {
            .flavor = COMMAND_REMOVE_ENTITY,
            .source = source,
            .specific.remove_entity = {
                    .id_path = path_from_cstring(id_path, alloc),
            },
    };

    return new_cmd;
}

/**
 * @brief Creates a command to subscribe an entity to some event.
 *
 * @param[in] source Entity that sent the command.
 * @param[in] event_name Name of the event the entity subscribes its callback to.
 * @param[in] callback Registered callback.
 * @param[inout] alloc Allocator used for the allocation of the command.
 * @return A fresh command to be queued.
 */
command command_create_subscribe_to_event(entity *source, const char *event_name, void (*callback)(void *entity_data, void *event_data), allocator alloc)
{
    command new_cmd = { 0u };

    if (!source || !event_name || !callback) {
        return (command) { .flavor = COMMAND_INVALID };
    }

    new_cmd = (command) {
            .flavor = COMMAND_SUBSCRIBE_TO_EVENT,
            .source = source,
            .specific.subscribe_to_event = {
                    .target_event_name = identifier_from_cstring(event_name, alloc),
                    .subscribed = source,
                    .callback = callback,
            },
    };

    return new_cmd;
}

// -------------------------------------------------------------------------------------------------

/**
 * @brief Releases the memory held by a command and zero-out its contents.
 *
 * @param[inout] cmd Destroyed command.
 * @param[inout] alloc Allocator used for the destruction of the command.
 */
void command_destroy(command *cmd, allocator alloc)
{
    if (!cmd) {
        return;
    }

    switch (cmd->flavor) {
    case COMMAND_ADD_ENTITY:
        range_destroy_dynamic(alloc, &range_to_any(cmd->specific.add_entity.id));
        path_destroy(&(cmd->specific.add_entity.id_path), alloc);
        entity_core_copy_destroy(&(cmd->specific.add_entity.template), alloc);
        break;

    case COMMAND_REMOVE_ENTITY:
        path_destroy(&(cmd->specific.remove_entity.id_path), alloc);
        break;

    case COMMAND_SUBSCRIBE_TO_EVENT:
        range_destroy_dynamic(alloc, &range_to_any(cmd->specific.subscribe_to_event.target_event_name));
        break;

    default:
        break;
    }

    *cmd = (command) { 0u };
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param alloc
 * @return
 */
command_queue *command_queue_create(allocator alloc)
{
    command_queue *new_queue = NULL;

    new_queue = alloc.malloc(alloc, sizeof(*new_queue));

    if (new_queue) {
        *new_queue = (command_queue) {
            .queue_impl = range_create_dynamic(alloc, sizeof(*new_queue->queue_impl->data), TARASQUE_COLLECTIONS_START_SIZE),
        };
    }

    return new_queue;
}

/**
 * @brief
 *
 * @param queue
 * @param alloc
 */
void command_queue_destroy(command_queue **queue, allocator alloc)
{
    if (!queue || !*queue) {
        return;
    }

    for (size_t i = 0u ; i < (*queue)->queue_impl->length ; i++) {
        command_destroy((*queue)->queue_impl->data + i, alloc);
    }

    range_destroy_dynamic(alloc, &range_to_any((*queue)->queue_impl));
    alloc.free(alloc, *queue);
    *queue = NULL;
}

// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param queue
 * @param source
 * @param cmd
 * @param alloc
 */
void command_queue_append(command_queue *queue, command cmd, allocator alloc)
{
    if (!queue || (cmd.flavor == COMMAND_INVALID)) {
        return;
    }

    queue->queue_impl = range_ensure_capacity(alloc, range_to_any(queue->queue_impl), 1);
    range_insert_value(range_to_any(queue->queue_impl), queue->queue_impl->length, &cmd);
}

/**
 * @brief
 *
 * @param queue
 * @return command
 */
command command_queue_pop_front(command_queue *queue)
{
    command popped_command = { 0u };

    if (!queue || !queue->queue_impl || (queue->queue_impl->length == 0u)) {
        return (command) { .flavor = COMMAND_INVALID };
    }

    popped_command = queue->queue_impl->data[0u];
    range_remove(range_to_any(queue->queue_impl), 0u);

    return popped_command;
}

/**
 * @brief
 *
 * @param queue
 * @return
 */
size_t command_queue_length(const command_queue *queue)
{
    if (!queue || !queue->queue_impl) {
        return 0u;
    }

    return queue->queue_impl->length;
}

// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param queue
 * @param target
 * @param alloc
 */
void command_queue_remove_commands_of(command_queue *queue, entity *target, allocator alloc)
{
    size_t pos = 0u;

    if (!queue || !target) {
        return;
    }

    while (pos < queue->queue_impl->length) {
        if (queue->queue_impl->data[pos].source == target) {
            command_destroy(queue->queue_impl->data + pos, alloc);
            range_remove(range_to_any(queue->queue_impl), pos);
        } else {
            pos += 1;
        }
    }
}
