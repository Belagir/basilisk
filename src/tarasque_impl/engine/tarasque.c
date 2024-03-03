

#include <signal.h>
#include <stdlib.h>
#include <time.h>

#include "../common.h"

#include "../command/command.h"
#include "../entity/entity.h"

/**
 * @brief
 *
 */
typedef struct tarasque_engine {
    command_queue *commands;
    // TODO : event_stack *events;
    entity *root_entity;
    // TODO : event_broker *pub_sub

    bool should_quit;

    allocator alloc;
} tarasque_engine;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/*  */
static void tarasque_engine_process_command(tarasque_engine *handle, command cmd);
/*  */
static void tarasque_engine_process_command_add_entity(tarasque_engine *handle, command_add_entity cmd);

/*  */
static void tarasque_engine_frame_step_entities(tarasque_engine *handle, f32 elapsed_time);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

volatile sig_atomic_t shared_interrupt_flag = 0;
static void cockatrice_engine_int_handler(int val) {
    if (val != SIGINT) {
        return;
    }

    if (!shared_interrupt_flag) {
        shared_interrupt_flag = 1;
    } else {
        exit(-1);
    }
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

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
                .root_entity = entity_create(identifier_root, (entity_template) { 0u }, used_alloc),
                .should_quit = false,
        };
    }

    signal(SIGINT, &cockatrice_engine_int_handler);

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

    entity_destroy_children((*handle)->root_entity, used_alloc);

    command_queue_destroy(&(*handle)->commands, used_alloc);
    entity_destroy(&(*handle)->root_entity, used_alloc);

    used_alloc.free(used_alloc, *handle);
    *handle = NULL;
}

/**
 * @brief
 *
 * @param handle
 * @param fps
 */
void tarasque_engine_run(tarasque_engine *handle, int fps) {
    f64 frame_delay = { 0. };

    if (!handle || (fps == 0u)) {
        return;
    }

    handle->should_quit = false;
    frame_delay = 1000. / (f64) fps;

    do {
        handle->should_quit = (shared_interrupt_flag == 1);

        while (command_queue_length(handle->commands) > 0u) {
            tarasque_engine_process_command(handle, command_queue_pop_front(handle->commands));
        }

        tarasque_engine_frame_step_entities(handle, (f32) frame_delay);

        (void) nanosleep(&(struct timespec) { .tv_nsec = (long) (frame_delay * 1000000.f) }, NULL);
    } while (!handle->should_quit);
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param handle
 * @param str_path
 * @param str_id
 * @param
 */
void tarasque_engine_add_entity(tarasque_engine *handle, const char *str_path, const char *str_id, entity_template template)
{

    if (!handle || !str_path || !str_id) {
        // TODO : log failure
        return;
    }

    command_queue_append(handle->commands, command_create_add_entity(handle->root_entity, str_path, str_id, template, handle->alloc), handle->alloc);
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param handle
 * @param cmd
 */
static void tarasque_engine_process_command(tarasque_engine *handle, command cmd)
{
    if (!handle) {
        return;
    }

    switch (cmd.flavor)
    {
    case COMMAND_ADD_ENTITY:
        tarasque_engine_process_command_add_entity(handle, cmd.specific.add_entity);
        break;

    default:
        break;
    }

    command_destroy(&cmd, handle->alloc);
}

/**
 * @brief
 *
 * @param handle
 * @param cmd
 */
static void tarasque_engine_process_command_add_entity(tarasque_engine *handle, command_add_entity cmd)
{
    entity *new_entity = NULL;
    entity *found_parent = NULL;

    if (!handle) {
        return;
    }

    found_parent = entity_get_child(handle->root_entity, cmd.id_path);

    if (found_parent) {
        new_entity = entity_create(cmd.id, cmd.template, handle->alloc);
        entity_add_child(found_parent, new_entity, handle->alloc);
    }
}

/**
 * @brief 
 * 
 * @param handle 
 * @param elapsed_time 
 */
static void tarasque_engine_frame_step_entities(tarasque_engine *handle, f32 elapsed_ms)
{
    entity_range *all_entities = NULL;

    if (!handle) {
        return;
    }

    all_entities = entity_get_children(handle->root_entity, handle->alloc);

    if (!all_entities) {
        return;
    }

    for (size_t i = 0u ; i < all_entities->length ; i++) {
        entity_step_frame(all_entities->data[i], elapsed_ms, handle);
    }

    range_destroy_dynamic(handle->alloc, &range_to_any(all_entities));
}
