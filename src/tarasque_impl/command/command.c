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
    RANGE(command) *queue_impl;
} command_queue;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Creates a command to remove an entity from the game tree.
 *
 * @param[in] source Entity that sent the command and to be removed.
 * @param[inout] alloc Allocator used for the allocation of the command.
 * @return A fresh command to be queued.
 */
command command_create_remove_entity(tarasque_engine_entity *source, allocator alloc)
{
    command new_cmd = { 0u };

    if (!source) {
        return (command) { .flavor = COMMAND_INVALID };
    }

    new_cmd = (command) {
            .flavor = COMMAND_REMOVE_ENTITY,
            .source = source,
            .specific.remove_entity = {
                    .removed = source,
            },
    };

    return new_cmd;
}

/**
 * @brief Creates a command to subscribe an entity to some event.
 *
 * @param[in] source Entity that sent the command.
 * @param[in] event_name Name (copied) of the event the entity subscribes its callback to.
 * @param[in] subscription_data Callback information.
 * @param[inout] alloc Allocator used for the allocation of the command.
 * @return A fresh command to be queued.
 */
command command_create_subscribe_to_event(tarasque_engine_entity *source, const char *event_name, tarasque_specific_event_subscription subscription_data, allocator alloc)
{
    command new_cmd = { 0u };

    if (!source || !event_name || !subscription_data.callback) {
        return (command) { .flavor = COMMAND_INVALID };
    }

    new_cmd = (command) {
            .flavor = COMMAND_SUBSCRIBE_TO_EVENT,
            .source = source,
            .specific.subscribe_to_event = {
                    .target_event_name = identifier_from_cstring(event_name, alloc),
                    .subscribed = source,
                    .subscription_data = subscription_data,
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

    case COMMAND_REMOVE_ENTITY:
        break;

    case COMMAND_SUBSCRIBE_TO_EVENT:
        range_destroy_dynamic(alloc, &RANGE_TO_ANY(cmd->specific.subscribe_to_event.target_event_name));
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
 * @brief Creates a new command queue on the heap and returns a pointer to it.
 *
 * @param[inout] alloc Allocator used for the memory queue allocation.
 * @return command_queue *
 */
command_queue *command_queue_create(allocator alloc)
{
    command_queue *new_queue = NULL;

    new_queue = alloc.malloc(alloc, sizeof(*new_queue));

    if (new_queue) {
        *new_queue = (command_queue) {
            .queue_impl = range_create_dynamic(alloc, sizeof(*new_queue->queue_impl->data), TARASQUE_COLLECTIONS_START_LENGTH),
        };
    }

    return new_queue;
}

/**
 * @brief Destroys a previously allocated command queue and nullifies the passed pointer.
 *
 * @param[inout] queue Target destroyed queue.
 * @param[inout] alloc Allocator used to release the memory.
 */
void command_queue_destroy(command_queue **queue, allocator alloc)
{
    if (!queue || !*queue) {
        return;
    }

    for (size_t i = 0u ; i < (*queue)->queue_impl->length ; i++) {
        command_destroy((*queue)->queue_impl->data + i, alloc);
    }

    range_destroy_dynamic(alloc, &RANGE_TO_ANY((*queue)->queue_impl));
    alloc.free(alloc, *queue);
    *queue = NULL;
}

// -------------------------------------------------------------------------------------------------

/**
 * @brief Adds a command at the end of a queue. The queue assumes ownership of the command.
 *
 * @param[inout] queue Command queue to be extended.
 * @param[in] cmd Queued command.
 * @param[inout] alloc Allocator used to ensure the queue has enough memory space.
 */
void command_queue_append(command_queue *queue, command cmd, allocator alloc)
{
    if (!queue || (cmd.flavor == COMMAND_INVALID)) {
        return;
    }

    queue->queue_impl = range_ensure_capacity(alloc, RANGE_TO_ANY(queue->queue_impl), 1);
    range_insert_value(RANGE_TO_ANY(queue->queue_impl), queue->queue_impl->length, &cmd);
}

/**
 * @brief Returns the command at the front of a queue, i.e. the oldest command. The command is removed from the queue.
 *
 * @param[inout] queue Target command queue.
 * @return command
 */
command command_queue_pop_front(command_queue *queue)
{
    command popped_command = { 0u };

    if (!queue || !queue->queue_impl || (queue->queue_impl->length == 0u)) {
        return (command) { .flavor = COMMAND_INVALID };
    }

    popped_command = queue->queue_impl->data[0u];
    range_remove(RANGE_TO_ANY(queue->queue_impl), 0u);

    return popped_command;
}

/**
 * @brief Returns the number of commands stored in the queue.
 *
 * @param[in] queue Queue examined.
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
 * @brief Destroys and removes all commands that have as source some specific entity.
 *
 * @param[inout] queue Traget queue.
 * @param[in] target Entity the function must remove commands of.
 * @param[inout] alloc Allocator used for the commands destruction.
 */
void command_queue_remove_commands_of(command_queue *queue, tarasque_engine_entity *target, allocator alloc)
{
    size_t pos = 0u;

    if (!queue || !target) {
        return;
    }

    while (pos < queue->queue_impl->length) {
        if (queue->queue_impl->data[pos].source == target) {
            command_destroy(queue->queue_impl->data + pos, alloc);
            range_remove(RANGE_TO_ANY(queue->queue_impl), pos);
        } else {
            pos += 1;
        }
    }
}
