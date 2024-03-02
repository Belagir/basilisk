
#include "command.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
typedef struct command_queue {
    range(command) *queue_impl;
} command_queue;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief 
 * 
 * @param source 
 * @param id_path 
 * @param id 
 * @param  
 * @param alloc 
 * @return 
 */
command command_create_add_entity(entity *source, char *id_path, char *id, entity_template template, allocator alloc)
{
    command new_cmd = { 0u };

    if (!id_path || !id) {
        return (command) { .flavor = COMMAND_INVALID };
    }

    new_cmd = (command) {
            .flavor = COMMAND_ADD_ENTITY,
            .source = source,
            .cmd.add_entity = (command_add_entity) { 
                    .id = identifier_from_cstring(id, alloc),
                    .id_path = identifier_from_cstring(id_path, alloc),
                    .template = entity_template_copy_create(template, alloc),
             },
    };

    return new_cmd;
}

// -------------------------------------------------------------------------------------------------

/**
 * @brief 
 * 
 * @param cmd 
 * @param alloc 
 */
void command_destroy(command *cmd, allocator alloc)
{
    if (!cmd) {
        return;
    }

    switch (cmd->flavor) {
    case COMMAND_ADD_ENTITY:
        range_destroy_dynamic(alloc, &range_to_any(cmd->cmd.add_entity.id));
        range_destroy_dynamic(alloc, &range_to_any(cmd->cmd.add_entity.id_path));
        entity_template_copy_destroy(&(cmd->cmd.add_entity.template), alloc);
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

    queue->queue_impl = range_ensure_capacity(alloc, range_to_any(queue->queue_impl));
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
size_t command_queue_length(command_queue *queue)
{
    if (!queue || !queue->queue_impl) {
        return 0u;
    }

    return queue->queue_impl->length;
}
