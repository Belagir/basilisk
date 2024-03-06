

#include <signal.h>
#include <stdlib.h>
#include <time.h>

#include "../common.h"

#include "../command/command.h"
#include "../entity/entity.h"
#include "../event/event.h"

/**
 * @brief
 *
 */
typedef struct tarasque_engine {
    command_queue *commands;
    event_stack *events;
    event_broker *pub_sub;

    entity *root_entity;
    entity *current_entity;

    bool should_quit;

    allocator alloc;
} tarasque_engine;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/*  */
static void tarasque_engine_full_destroy_entity(tarasque_engine *handle, entity *target);

/*  */
static void tarasque_engine_process_command(tarasque_engine *handle, command cmd);
/*  */
static void tarasque_engine_process_command_add_entity(tarasque_engine *handle, entity *subject, command_add_entity cmd);
/*  */
static void tarasque_engine_process_command_remove_entity(tarasque_engine *handle, entity *subject, command_remove_entity cmd);
/*  */
static void tarasque_engine_process_command_subscribe_to_event(tarasque_engine *handle, command_subscribe_to_event cmd);

static void tarasque_engine_process_event(tarasque_engine *handle, event processed_event);

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
                .commands = command_queue_create(used_alloc),
                .events = event_stack_create(used_alloc),
                .pub_sub = event_broker_create(used_alloc),

                .root_entity = entity_create(identifier_root, (entity_core) { 0u }, NULL, used_alloc),
                .current_entity = NULL,

                .should_quit = false,

                .alloc = used_alloc,
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
    allocator used_alloc = { 0u };

    if (!handle || !*handle) {
        return;
    }

    used_alloc = (*handle)->alloc;

    event_broker_destroy(&(*handle)->pub_sub, used_alloc);
    event_stack_destroy(&(*handle)->events, used_alloc);
    command_queue_destroy(&(*handle)->commands, used_alloc);

    entity_destroy_children((*handle)->root_entity, NULL, used_alloc);
    entity_destroy(&(*handle)->root_entity, NULL, used_alloc);

    used_alloc.free(used_alloc, *handle);
    *handle = NULL;
}

// -------------------------------------------------------------------------------------------------

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

        while (event_stack_length(handle->events) > 0u) {
            tarasque_engine_process_event(handle, event_stack_pop(handle->events));
        }

        tarasque_engine_frame_step_entities(handle, (f32) frame_delay);

        (void) nanosleep(&(struct timespec) { .tv_nsec = (long) (frame_delay * 1000000.f) }, NULL);
    } while (!handle->should_quit);
}

/**
 * @brief
 *
 * @param handle
 */
void tarasque_engine_quit(tarasque_engine *handle)
{
    if (!handle) {
        return;
    }

    handle->should_quit = true;
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
 * @param template
 */
void tarasque_engine_add_entity(tarasque_engine *handle, const char *str_path, const char *str_id, entity_core template)
{

    if (!handle || !str_path || !str_id) {
        // TODO : log failure
        return;
    }

    command_queue_append(handle->commands, command_create_add_entity(handle->current_entity, str_path, str_id, template, handle->alloc), handle->alloc);
    // TODO : log failure to create command
}

/**
 * @brief
 *
 * @param handle
 * @param str_path
 */
void tarasque_engine_remove_entity(tarasque_engine *handle, const char *str_path)
{
    if (!handle || !str_path) {
        // TODO : log failure
        return;
    }

    command_queue_append(handle->commands, command_create_remove_entity(handle->current_entity, str_path, handle->alloc), handle->alloc);
    // TODO : log failure to create command
}

/**
 * @brief
 *
 * @param handle
 * @param str_id
 * @param callback
 */
void tarasque_engine_subscribe_to_event(tarasque_engine *handle,  const char *str_id, void (*callback)(void *entity_data, void *event_data))
{
    if (!handle || !str_id) {
        // TODO : log failure
        return;
    }

    command_queue_append(handle->commands, command_create_subscribe_to_event(handle->current_entity, str_id, callback, handle->alloc), handle->alloc);
    // TODO : log failure to create command
}

/**
 * @brief
 *
 * @param handle
 * @param str_event_name
 * @param event_data_size
 * @param event_data
 * @param is_detached
 */
void tarasque_engine_stack_event(tarasque_engine *handle, const char *str_event_name, size_t event_data_size, void *event_data, bool is_detached)
{
    entity *source_entity = NULL;

    if (!handle || !str_event_name) {
        // TODO : log failure
        return;
    }

    if (!is_detached) {
        source_entity = handle->current_entity;
    }

    event_stack_push(handle->events, source_entity, str_event_name, event_data_size, event_data, handle->alloc);
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param handle
 * @param current_entity
 * @return tarasque_engine*
 */
tarasque_engine *tarasque_engine_for(tarasque_engine *handle, entity *current_entity)
{
    if (!handle) {
        return NULL;
    }

    handle->current_entity = current_entity;
    return handle;
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param handle
 * @param target
 */
static void tarasque_engine_full_destroy_entity(tarasque_engine *handle, entity *target)
{
    entity_range *all_children = NULL;

    if (!handle || !target) {
        return;
    }

    entity_deparent(target);

    all_children = entity_get_children(target, handle->alloc);
    for (size_t i = 0u ; i < all_children->length ; i++) {
        event_stack_remove_events_of(handle->events, all_children->data[i], handle->alloc);
        command_queue_remove_commands_of(handle->commands, all_children->data[i], handle->alloc);
        event_broker_unsubscribe(handle->pub_sub, all_children->data[i], NULL, NULL, handle->alloc);
        entity_destroy(&(all_children->data[i]), handle, handle->alloc);
    }
    range_destroy_dynamic(handle->alloc, &range_to_any(all_children));

    event_stack_remove_events_of(handle->events, target, handle->alloc);
    command_queue_remove_commands_of(handle->commands, target, handle->alloc);
    event_broker_unsubscribe(handle->pub_sub, target, NULL, NULL, handle->alloc);
    entity_destroy(&target, handle, handle->alloc);
}

/**
 * @brief
 *
 * @param handle
 * @param cmd
 */
static void tarasque_engine_process_command(tarasque_engine *handle, command cmd)
{
    entity *subject = NULL;

    if (!handle) {
        return;
    }

    subject = cmd.source;
    if (!subject) {
        subject = handle->root_entity;
    }

    switch (cmd.flavor) {
    case COMMAND_ADD_ENTITY:
        tarasque_engine_process_command_add_entity(handle, subject, cmd.specific.add_entity);
        break;
    case COMMAND_REMOVE_ENTITY:
        tarasque_engine_process_command_remove_entity(handle, subject, cmd.specific.remove_entity);
        break;
    case COMMAND_SUBSCRIBE_TO_EVENT:
        tarasque_engine_process_command_subscribe_to_event(handle, cmd.specific.subscribe_to_event);
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
static void tarasque_engine_process_command_add_entity(tarasque_engine *handle, entity *subject, command_add_entity cmd)
{
    entity *new_entity = NULL;
    entity *found_parent = NULL;

    if (!handle) {
        return;
    }

    found_parent = entity_get_child(subject, cmd.id_path);

    if (found_parent) {
        // TODO : enforce unique entity identifier among children
        new_entity = entity_create(cmd.id, cmd.template, handle, handle->alloc);
        entity_add_child(found_parent, new_entity, handle->alloc);
    } else {
        // TODO : log failure
    }
}

/**
 * @brief
 *
 * @param handle
 * @param cmd
 */
static void tarasque_engine_process_command_remove_entity(tarasque_engine *handle, entity *subject, command_remove_entity cmd)
{
    entity *found_entity = NULL;

    if (!handle) {
        return;
    }

    found_entity = entity_get_child(subject, cmd.id_path);

    if (found_entity == handle->root_entity) {
        // TODO : log failure
        return;
    }

    if (!found_entity) {
        // TODO : log failure
        return;
    }

    tarasque_engine_full_destroy_entity(handle, found_entity);
}

/**
 * @brief
 *
 * @param handle
 * @param cmd
 */
static void tarasque_engine_process_command_subscribe_to_event(tarasque_engine *handle, command_subscribe_to_event cmd)
{
    if (!handle) {
        return;
    }

    event_broker_subscribe(handle->pub_sub, cmd.subscribed, cmd.target_event_name, cmd.callback, handle->alloc);
}

/**
 * @brief
 *
 * @param handle
 * @param processed_event
 */
static void tarasque_engine_process_event(tarasque_engine *handle, event processed_event)
{
    if (!handle) {
        return;
    }

    event_broker_publish(handle->pub_sub, processed_event);

    event_destroy(&processed_event, handle->alloc);
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

    // TODO : children collections should be updated on entities change, not every frame
    all_entities = entity_get_children(handle->root_entity, handle->alloc);

    if (!all_entities) {
        return;
    }

    for (size_t i = 0u ; i < all_entities->length ; i++) {
        entity_step_frame(all_entities->data[i], elapsed_ms, handle);
    }

    range_destroy_dynamic(handle->alloc, &range_to_any(all_entities));
}
