
#include "../common.h"

#include "../command/command.h"

/**
 * @brief 
 * 
 */
typedef struct tarasque_engine {
    command_queue *commands;
    // TODO event_stack *events;
    entity *root_entity;
    allocator alloc;
} tarasque_engine;

/**
 * @brief 
 *  
 */
tarasque_engine *tarasque_engine_create(void)
{
    allocator used_alloc = make_system_allocator();
    tarasque_engine *new_engine = NULL;

    new_engine = used_alloc.malloc(used_alloc, sizeof(*new_engine));

    if (new_engine) {
        *new_engine = (tarasque_engine) {
                .alloc = used_alloc,
                .commands = command_queue_create(used_alloc),
        };
    }

    return new_engine;
}

/**
 * @brief 
 * 
 * @param handle 
 */
void tarasque_engine_destroy(tarasque_engine **handle)
{
    allocator used_alloc = make_system_allocator();
    
    if (!handle || !*handle) {
        return;
    }

    used_alloc = (*handle)->alloc;

    command_queue_destroy(&(*handle)->commands, used_alloc);

    used_alloc.free(used_alloc, *handle);
    *handle = NULL;
}

/**
 * @brief 
 * 
 * @param handle 
 * @param str_path 
 * @param str_id 
 * @param  
 */
void tarasque_engine_add_entity(tarasque_engine *handle, char *str_path, char *str_id, entity_template template)
{

    if (!handle || !str_path || !str_id) {
        // TODO : log failure
        return;
    }

    command_queue_append(handle->commands, command_create_add_entity(handle->root_entity, str_path, str_id, template, handle->alloc), handle->alloc);
}
