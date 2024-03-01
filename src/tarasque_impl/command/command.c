
#include "command.h"

/**
 * @brief 
 * 
 */
typedef struct command_queue {
    range(command) *queue_impl;
} command_queue;

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

    range_destroy_dynamic(alloc, &range_to_any((*queue)->queue_impl));
    alloc.free(alloc, *queue);
    *queue = NULL;
}
