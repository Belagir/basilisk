
#include "../common.h"

/**
 * @brief 
 * 
 */
typedef struct tarasque_engine {
    // TODO command_queue *commands
    // TODO event_stack *events
    // TODO entity *root_entity
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

    used_alloc.free(used_alloc, *handle);
    *handle = NULL;
}

