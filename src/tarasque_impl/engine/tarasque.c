/**
 * @file tarasque.c
 * @author gabriel ()
 * @brief Implementation file for the engine main header @file tarasque.h and support header @file tarasque_impl.h.
 *
 * This file's responsability is to aggregate the implementation of core operations of the engine.
 *
 * @version 0.1
 * @date 2024-03-06
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <signal.h>
#include <stdlib.h>
#include <time.h>

#include <ustd/logging.h>

#include "tarasque_impl.h"

#include "../common.h"

#include "../command/command.h"
#include "../event/event.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Data layout of an engine instance. Every operation possible stems from one of the objects
 * stored in this struct : this is the central data structure of the engine, from which we can navigate
 * to any engine-owned memory.
 */
typedef struct tarasque_engine {
    /** Logger object to communicate to the outside world. */
    logger *logger;
    /** Command queue containing pending operations that will change the state of other collections. */
    command_queue *commands;
    /** Stack of events sent from entities to other entities. */
    event_stack *events;
    /** Publisher / subscriber object maintaining a collection of subscriptions of entities to events. */
    event_broker *pub_sub;

    /** Root of the game tree as an empty entity. */
    tarasque_entity *root_entity;

    /** Flag signaling wether the engine should exit or not the main loop. */
    bool should_quit;

    /** Allocator object used for all memory operations done by the engine instance. */
    allocator alloc;
} tarasque_engine;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* Removes an entity from the tree, cleaning all objects referencing to it, and does so for all its children. */
static void tarasque_engine_full_destroy_entity(tarasque_engine *handle, tarasque_entity *target);

// -------------------------------------------------------------------------------------------------

/* Processes a general command, applying its effects and then destroying it. */
static void tarasque_engine_process_command(tarasque_engine *handle, command cmd);
/* Processes a specific command to add an entity in the engine, changing the state of the game tree. */
static void tarasque_engine_process_command_add_entity(tarasque_engine *handle, tarasque_entity *subject, command_add_entity cmd);

static void tarasque_engine_process_command_graft(tarasque_engine *handle, tarasque_entity *subject, command_graft cmd);
/* Processes a specific command to remove an entity from the engine, changing the state of the game tree. */
static void tarasque_engine_process_command_remove_entity(tarasque_engine *handle, tarasque_entity *subject, command_remove_entity cmd);
/* Processes a specific command to subscribe an entity to an event, changing the state of the publisher / susbcriber collection. */
static void tarasque_engine_process_command_subscribe_to_event(tarasque_engine *handle, command_subscribe_to_event cmd);

// -------------------------------------------------------------------------------------------------

/* Processes an event, passing it to registered entities and then destroying it. */
static void tarasque_engine_process_event(tarasque_engine *handle, event processed_event);

// -------------------------------------------------------------------------------------------------

/* Steps all entities forward in time with their on_frame() callback. */
static void tarasque_engine_frame_step_entities(tarasque_engine *handle, f32 elapsed_time);

// -------------------------------------------------------------------------------------------------
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
 * @brief Creates an engine instance on the heap, and returns a pointer to its data on success.
 *
 * @return tarasque_engine*
 */
tarasque_engine *tarasque_engine_create(void)
{
    allocator used_alloc = make_system_allocator();
    tarasque_engine *new_engine = NULL;

    new_engine = used_alloc.malloc(used_alloc, sizeof(*new_engine));

    if (new_engine) {
        *new_engine = (tarasque_engine) {
                .logger = logger_create(stdout, LOGGER_ON_DESTROY_DO_NOTHING),

                .commands = command_queue_create(used_alloc),
                .events = event_stack_create(used_alloc),
                .pub_sub = event_broker_create(used_alloc),

                .root_entity = entity_create(identifier_root, (entity_user_data) { 0u }, new_engine, used_alloc),

                .should_quit = false,

                .alloc = used_alloc,
        };

        logger_log(new_engine->logger, LOGGER_SEVERITY_INFO, "Engine is ready.\n");
    }

    signal(SIGINT, &cockatrice_engine_int_handler);

    return new_engine;
}

/**
 * @brief Destroys memory held by an engine instance, releasing all memory held by it and nullifying the given pointer.
 *
 * @param[inout] handle double pointer to an engine instance.
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

    tarasque_engine_full_destroy_entity((*handle), (*handle)->root_entity);

    logger_log((*handle)->logger, LOGGER_SEVERITY_INFO, "Engine shut down.\n");


    logger_destroy(&(*handle)->logger);

    used_alloc.free(used_alloc, *handle);
    *handle = NULL;
}

/**
 * @brief
 *
 * @param handle
 * @return
 */
entity_data *tarasque_engine_root_entity(tarasque_engine *handle)
{
    if (!handle) {
        return NULL;
    }

    return handle->root_entity->data;
}

// -------------------------------------------------------------------------------------------------

/**
 * @brief Starts a main loop until an interupt signal is sent to the program or an entity flags the
 * engine to quit.
 *
 * During a frame, the engine will process all commands describing pending operations, then unwind the
 * event stack until it is empty, and finaly step all entities from the root of the tree to its leafs.
 *
 * @param[inout] handle Engine instance.
 * @param[in] fps Target frequency of the main loop.
 */
void tarasque_engine_run(tarasque_engine *handle, int fps) {
    f64 frame_delay = { 0. };

    if (!handle || (fps == 0u)) {
        return;
    }

    handle->should_quit = false;
    frame_delay = 1000. / (f64) fps;

    logger_log(handle->logger, LOGGER_SEVERITY_INFO, "Started the main loop at %d fps..\n", fps);

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
 * @brief Flags the engine to quit on the next frame.
 * The current frame will still finish before quitting.
 *
 * @param[inout] scene
 */
void tarasque_entity_scene_quit(entity_data *entity)
{
    if (!entity) {
        return;
    }

    tarasque_entity *full_entity = container_of(entity, tarasque_entity, data);

    full_entity->host_handle->should_quit = true;
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Queues a command to add an entity into the game tree.
 * If the function is called from a callback registered in the engine, the path is relative to the entity
 * under which the callback was registered. If this entity is removed before the operation is done, the
 * command is also removed.
 *
 * @param[inout] entity
 * @param[in] str_path String (copied) describing a '/'-delimited path to an entity that will be the parent of the new entity.
 * @param[in] str_id New entity's name (copied), must be unique in respect to its siblings and not contain the character '/'.
 * @param[in] user_data Basic entity information (copied).
 */
void tarasque_entity_scene_add_entity(entity_data *entity, const char *str_path, const char *str_id, entity_user_data user_data)
{
    if (!entity) {
        return;
    }

    command cmd = { 0u };
    tarasque_entity *full_entity = container_of(entity, tarasque_entity, data);
    tarasque_engine *handle = full_entity->host_handle;

    if (handle) {
        cmd = command_create_add_entity(full_entity, str_path, str_id, user_data, handle->alloc);
        command_queue_append(handle->commands, cmd, handle->alloc);
    }
}

/**
 * @brief Queue a command to remove an entity into the game tree.
 * If the function is called from a callback registered in the engine, the path is relative to the entity
 * under which the callback was registered. If this entity is removed before the operation is done, the
 * command is also removed.
 *
 * @param[inout] scene
 * @param[in] str_path tring (copied) describing a '/'-delimited path to the removeed entity.
 */
void tarasque_entity_scene_remove_entity(entity_data *entity, const char *str_path)
{
    if (!entity) {
        return;
    }

    command cmd = { 0u };
    tarasque_entity *full_entity = container_of(entity, tarasque_entity, data);
    tarasque_engine *handle = full_entity->host_handle;

    if (handle) {
        cmd = command_create_remove_entity(full_entity, str_path, handle->alloc);
        command_queue_append(handle->commands, cmd, handle->alloc);
    }
}

/**
 * @brief
 *
 * @param scene
 * @param str_path
 * @param str_id
 * @param graft_procedure
 * @param graft_args
 */
void tarasque_entity_scene_graft(entity_data *entity, const char *str_path, const char *str_id, graft_user_data graft_data)
{
    if (!entity) {
        return;
    }

    command cmd = { 0u };
    tarasque_entity *full_entity = container_of(entity, tarasque_entity, data);
    tarasque_engine *handle = full_entity->host_handle;

    if (handle) {
        cmd = command_create_graft(full_entity, str_path, str_id, graft_data, handle->alloc);
        command_queue_append(handle->commands, cmd, handle->alloc);
    }
}

/**
 * @brief Queue a command to subscribe an entity's callback to an event.
 * This function can only be called from an entity's registered callback. If this entity is removed
 * before the operation is done, the command is also removed.
 *
 * @param[inout] scene
 * @param[in] str_event_name Name (copied) of the event the entity wants to subscribe a callback to.
 * @param[in] callback Pointer to the callback that will receive the entity's data and event data.
 */
void tarasque_entity_scene_subscribe_to_event(entity_data *entity,  const char *str_event_name, event_subscription_user_data subscription_data)
{
    if (!entity) {
        return;
    }

    command cmd = { 0u };
    tarasque_entity *full_entity = container_of(entity, tarasque_entity, data);
    tarasque_engine *handle = full_entity->host_handle;

    if (handle) {
        cmd = command_create_subscribe_to_event(full_entity, str_event_name, subscription_data, handle->alloc);
        command_queue_append(handle->commands, cmd, handle->alloc);
    }
}

/**
 * @brief Immediately stacks a named event to be sent to all entities registered to the event's name.
 *
 * @param[inout] scene
 * @param[in] str_event_name Name (copied) of the event stacked.
 * @param[in] event_data_size Event's specific data size in bytes.
 * @param[in] event_data Event's specific data (copied).
 * @param[in] is_detached if set, the event will not be removed if the entity that sent it is removed itself.
 */
void tarasque_entity_scene_stack_event(entity_data *entity, const char *str_event_name, size_t event_data_size, void *event_data, bool is_detached)
{
    if (!entity) {
        return;
    }

    tarasque_entity *full_entity = container_of(entity, tarasque_entity, data);
    tarasque_engine *handle = full_entity->host_handle;

    if (is_detached) {
        event_stack_push(handle->events, handle->root_entity, str_event_name, event_data_size, event_data, handle->alloc);
    } else {
        event_stack_push(handle->events, full_entity, str_event_name, event_data_size, event_data, handle->alloc);
    }

}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Completely removes an entity and its children from everything in the engine.
 * Additionally of removing the entities from the tree, it will remove all relevant events, subscriptions
 * and commands linked to the entities. All of the entities memory is released.
 *
 * @param[inout] handle Engine handle.
 * @param[inout] target Entity to remove along its children.
 */
static void tarasque_engine_full_destroy_entity(tarasque_engine *handle, tarasque_entity *target)
{
    entity_range *all_children = NULL;

    if (!handle || !target) {
        return;
    }

    entity_deparent(target);

    all_children = entity_get_children(target, handle->alloc);
    for (size_t i = 0u ; i < all_children->length ; i++) {
        entity_deinit(all_children->data[i], handle);
        event_stack_remove_events_of(handle->events, all_children->data[i], handle->alloc);
        command_queue_remove_commands_of(handle->commands, all_children->data[i], handle->alloc);
        event_broker_unsubscribe_from_all(handle->pub_sub, all_children->data[i], handle->alloc);
        entity_destroy(&(all_children->data[i]), handle->alloc);
    }
    range_destroy_dynamic(handle->alloc, &range_to_any(all_children));

    entity_deinit(target, handle);
    event_stack_remove_events_of(handle->events, target, handle->alloc);
    command_queue_remove_commands_of(handle->commands, target, handle->alloc);
    event_broker_unsubscribe_from_all(handle->pub_sub, target, handle->alloc);
    entity_destroy(&target, handle->alloc);
}

// -------------------------------------------------------------------------------------------------

/**
 * @brief Processes a command to change the engine's state. The command will be destroyed.
 *
 * @param[inout] handle Engine handle.
 * @param[inout] cmd Command to process.
 */
static void tarasque_engine_process_command(tarasque_engine *handle, command cmd)
{
    tarasque_entity *subject = NULL;

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
    case COMMAND_GRAFT:
        tarasque_engine_process_command_graft(handle, subject, cmd.specific.graft);
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
 * @brief Processes a command trusted to be a command to add an entity to the game tree at some position.
 *
 * @param[inout] handle Engine handle.
 * @param[in] subject Entity that sent the command.
 * @param[in] cmd Command containing the addition data.
 */
static void tarasque_engine_process_command_add_entity(tarasque_engine *handle, tarasque_entity *subject, command_add_entity cmd)
{
    tarasque_entity *new_entity = NULL;
    tarasque_entity *found_parent = NULL;

    if (!handle) {
        return;
    }

    found_parent = entity_get_child(subject, cmd.id_path);

    if (!found_parent) {
        logger_log(handle->logger, LOGGER_SEVERITY_ERRO, "Could not find parent to add entity \"%s\".\n", cmd.id->data);
        return;

    } else if (entity_get_direct_child(found_parent, cmd.id) != NULL) {
        logger_log(handle->logger, LOGGER_SEVERITY_ERRO, "Parent \"%s\" already has child named \"%s\".\n", entity_get_name(found_parent)->data, cmd.id->data);
        return;
    }

    new_entity = entity_create(cmd.id, cmd.user_data, handle, handle->alloc);
    entity_add_child(found_parent, new_entity, handle->alloc);
    entity_init(new_entity, handle);

    logger_log(handle->logger, LOGGER_SEVERITY_INFO, "Added entity \"%s\" under parent \"%s\".\n", entity_get_name(new_entity)->data, entity_get_name(found_parent)->data);
}

/**
 * @brief
 *
 * @param handle
 * @param subject
 * @param cmd
 */
static void tarasque_engine_process_command_graft(tarasque_engine *handle, tarasque_entity *subject, command_graft cmd)
{
    tarasque_entity *graft_parent = NULL;
    tarasque_entity *graft_root = NULL;

    if (!handle) {
        return;
    }

    graft_parent = entity_get_child(subject, cmd.id_path);

    if (!graft_parent) {
        return;
    }

    graft_root = entity_create(cmd.id, (entity_user_data_copy) { 0u }, handle, handle->alloc);
    entity_add_child(graft_parent, graft_root, handle->alloc);
    entity_init(graft_parent, handle);

    cmd.graft_data.graft_procedure(graft_root->data, cmd.graft_data.args);

    logger_log(handle->logger, LOGGER_SEVERITY_INFO, "Added graft \"%s\" under parent \"%s\".\n", cmd.id->data, entity_get_name(graft_parent)->data);
}

/**
 * @brief Processes a command trusted to be a command to remove an entity at some position in the game tree.
 *
 * @param[inout] handle Engine handle.
 * @param[in] subject Entity that sent the command.
 * @param[in] cmd Command containing the removal data.
 */
static void tarasque_engine_process_command_remove_entity(tarasque_engine *handle, tarasque_entity *subject, command_remove_entity cmd)
{
    tarasque_entity *found_entity = NULL;

    if (!handle) {
        return;
    }

    found_entity = entity_get_child(subject, cmd.id_path);

    if (found_entity == handle->root_entity) {
        logger_log(handle->logger, LOGGER_SEVERITY_ERRO, "Cannot remove root entity.\n");
        return;
    }

    if (!found_entity) {
        logger_log(handle->logger, LOGGER_SEVERITY_WARN, "Could not find entity \"%s\" to remove.\n", entity_get_name(subject)->data);
        return;
    }

    logger_log(handle->logger, LOGGER_SEVERITY_INFO, "Removed entity \"%s\".\n", entity_get_name(subject)->data);
    tarasque_engine_full_destroy_entity(handle, found_entity);
}

/**
 * @brief Processes a command trusted to be a command to subscribe an entity to an event name.
 *
 * @param[inout] handle Engine handle.
 * @param[in] subject Entity that sent the command.
 * @param[in] cmd Command containing the subscribtion data.
 */
static void tarasque_engine_process_command_subscribe_to_event(tarasque_engine *handle, command_subscribe_to_event cmd)
{
    if (!handle) {
        return;
    }

    event_broker_subscribe(handle->pub_sub, cmd.subscribed, cmd.target_event_name, cmd.subscription_data, handle->alloc);
    logger_log(handle->logger, LOGGER_SEVERITY_INFO, "Entity \"%s\" subscribed callback %#010x to event \"%s\".\n", entity_get_name(cmd.subscribed)->data, cmd.subscription_data, cmd.target_event_name->data);
}

// -------------------------------------------------------------------------------------------------

/**
 * @brief Sends an event to all entities registered to the event's name. The event is destroyed.
 *
 * @param[inout] handle Engine handle.
 * @param[inout] processed_event Event sent to entities.
 */
static void tarasque_engine_process_event(tarasque_engine *handle, event processed_event)
{
    if (!handle) {
        return;
    }

    event_broker_publish(handle->pub_sub, processed_event, handle);

    event_destroy(&processed_event, handle->alloc);
}

// -------------------------------------------------------------------------------------------------

/**
 * @brief Invoques all on_frame() callbacks found in the game tree's entities, from the root of the tree
 * to the leafs.
 *
 * @param[inout] handle Engine handle.
 * @param[in] elapsed_time milliseconds elapsed since the last time this function was executed.
 */
static void tarasque_engine_frame_step_entities(tarasque_engine *handle, f32 elapsed_ms)
{
    entity_range *all_entities = NULL;

    if (!handle) {
        return;
    }

    // TODO (low priority because of possible big impact on implementation) : children collections should be updated on entities change, not every frame
    all_entities = entity_get_children(handle->root_entity, handle->alloc);

    if (!all_entities) {
        return;
    }

    for (size_t i = 0u ; i < all_entities->length ; i++) {
        entity_step_frame(all_entities->data[i], elapsed_ms, handle);
    }

    range_destroy_dynamic(handle->alloc, &range_to_any(all_entities));
}
