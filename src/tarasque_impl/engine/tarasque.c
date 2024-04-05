/**
 * @file tarasque.c
 * @author gabriel ()
 * @brief Implementation file for the engine main header tarasque_bare.h and support header tarasque_impl.h.
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

#include <tarasque_bare.h>

#include "../common.h"

#include "../command/command.h"
#include "../entity/entity.h"
#include "../event/event.h"
#include "../resource/resource.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Data layout of an engine instance. Every operation possible stems from one of the objects
 * stored in this struct : this is the central data structure of the engine, from which we can navigate
 * to any engine-owned memory.
 */
typedef struct tarasque_engine {
    /** Allocator object used for all memory operations done by the engine instance. */
    allocator alloc;

    /** Logger object to communicate to the outside world. */
    logger *logger;
    /** Command queue containing pending operations that will change the state of other collections. */
    command_queue *commands;
    /** Stack of events sent from entities to other entities. */
    event_stack *events;
    /** Publisher / subscriber object maintaining a collection of subscriptions of entities to events. */
    event_broker *pub_sub;
    /** Resource manager object to load / unload files from the filesystem. */
    resource_manager *res_manager;

    /** Root of the game tree as an empty entity. */
    tarasque_engine_entity *root_entity;

    /** Buffer of references to entities to use for the main loop. */
    tarasque_engine_entity_range *active_entities;
    /** Flags that the active entities buffer needs to be reloaded. */
    bool update_active_entities;

    /** Flag signaling wether the engine should exit or not the main loop. */
    bool should_quit;
} tarasque_engine;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* Removes an entity from the tree, cleaning all objects referencing to it, and does so for all its children. */
static void tarasque_engine_annihilate_entity_and_chilren(tarasque_engine *handle, tarasque_engine_entity *target);
/* Removes an entity from the tree and cleaning all objects referencing to it */
static void tarasque_engine_annihilate_entity(tarasque_engine *handle, tarasque_engine_entity *target);

// -------------------------------------------------------------------------------------------------

/* Processes a general command, applying its effects and then destroying it. */
static void tarasque_engine_process_command(tarasque_engine *handle, command cmd);
/* Processes a specific command to remove an entity from the engine, changing the state of the game tree. */
static void tarasque_engine_process_command_remove_entity(tarasque_engine *handle, tarasque_engine_entity *subject, command_remove_entity *cmd);
/* Processes a specific command to subscribe an entity to an event, changing the state of the publisher / susbcriber collection. */
static void tarasque_engine_process_command_subscribe_to_event(tarasque_engine *handle, command_subscribe_to_event *cmd);

// -------------------------------------------------------------------------------------------------

/* Processes an event, passing it to registered entities and then destroying it. */
static void tarasque_engine_process_event(tarasque_engine *handle, event processed_event);

// -------------------------------------------------------------------------------------------------

/* Steps all entities forward in time with their on_frame() callback. */
static void tarasque_engine_frame_step_entities(tarasque_engine *handle, f32 elapsed_time);

// -------------------------------------------------------------------------------------------------

/* Updates the active entities buffer if needed. */
static void tarasque_engine_update_active_entities(tarasque_engine *handle);

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

    signal(SIGINT, &cockatrice_engine_int_handler);

    new_engine = used_alloc.malloc(used_alloc, sizeof(*new_engine));
    if (new_engine) {
        *new_engine = (tarasque_engine) {
                .alloc = used_alloc,

                .logger = logger_create(stdout, LOGGER_ON_DESTROY_DO_NOTHING),

                .commands    = command_queue_create(used_alloc),
                .events      = event_stack_create(used_alloc),
                .pub_sub     = event_broker_create(used_alloc),
                .res_manager = resource_manager_create(used_alloc),

                .root_entity = tarasque_engine_entity_create(identifier_root, (tarasque_specific_entity) { 0u }, new_engine, used_alloc),

                .active_entities = NULL,
                .update_active_entities = false,

                .should_quit = false,
        };

        logger_log(new_engine->logger, LOGGER_SEVERITY_INFO, "Engine is ready.\n");
    }

    return new_engine;
}

/**
 * @brief Destroys memory held by an engine instance, releasing all memory held by it and nullifying
 * the given pointer.
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

    if ((*handle)->active_entities) {
        range_destroy_dynamic(used_alloc, &RANGE_TO_ANY((*handle)->active_entities));
    }

    resource_manager_destroy(&(*handle)->res_manager, used_alloc);
    event_broker_destroy(&(*handle)->pub_sub, used_alloc);
    event_stack_destroy(&(*handle)->events, used_alloc);
    command_queue_destroy(&(*handle)->commands, used_alloc);

    tarasque_engine_annihilate_entity_and_chilren((*handle), (*handle)->root_entity);

    logger_log((*handle)->logger, LOGGER_SEVERITY_INFO, "Engine shut down.\n");

    logger_destroy(&(*handle)->logger);

    used_alloc.free(used_alloc, *handle);
    *handle = NULL;
}

/**
 * @brief Returns the data pointer of the root entity to be used with tarasque_entity_*() functions.
 * The pointer itself points to unallocated memory and cannot be dereferenced.
 *
 * @param[in] handle Handle to an engine instance. If NULL, the functions returns NULL.
 * @return tarasque_entity *
 */
tarasque_entity *tarasque_engine_root_entity(tarasque_engine *handle)
{
    if (!handle) {
        return NULL;
    }

    return tarasque_engine_entity_get_specific_data(handle->root_entity);
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

        tarasque_engine_update_active_entities(handle);

        tarasque_engine_frame_step_entities(handle, (f32) frame_delay);

        (void) nanosleep(&(struct timespec) { .tv_nsec = (long) (frame_delay * 1000000.f) }, NULL);
    } while (!handle->should_quit);
}

/**
 * @brief Flags the engine to quit on the next frame.
 * The current frame will still finish before quitting.
 *
 * @param[inout] entity entity querying the end of process.
 */
void tarasque_entity_quit(tarasque_entity *entity)
{
    if (!entity) {
        return;
    }

    tarasque_engine_entity *full_entity = tarasque_engine_entity_get_containing_full_entity(entity);

    tarasque_engine_entity_get_host_engine_handle(full_entity)->should_quit = true;
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Immediately builds an entity and adds it as a child to another, and returns a pointer to the new entity.
 *
 * @param[in] entity Entity soon-to-be parent.
 * @param[in] str_id Name (copied) of the new entity.
 * @param[in] user_data Definition (copied) of the entity.
 * @return tarasque_entity *
 */
tarasque_entity *tarasque_entity_add_child(tarasque_entity *entity, const char *str_id, tarasque_specific_entity user_data)
{
    tarasque_entity *new_entity = NULL;
    identifier *new_entity_id = NULL;

    tarasque_engine_entity *full_entity = tarasque_engine_entity_get_containing_full_entity(entity);
    tarasque_engine *handle = tarasque_engine_entity_get_host_engine_handle(full_entity);

    if (!full_entity || !handle) {
        return NULL;
    }

    new_entity_id = identifier_from_cstring(str_id, handle->alloc);

    if (!new_entity_id) {
        return NULL;
    }

    while (tarasque_engine_entity_get_direct_child(full_entity, new_entity_id) != NULL) {
        identifier_increment(&new_entity_id, handle->alloc);
    }

    new_entity = tarasque_engine_entity_create(new_entity_id, user_data, handle, handle->alloc);
    tarasque_engine_entity_add_child(full_entity, new_entity, handle->alloc);
    tarasque_engine_entity_init(new_entity);

    logger_log(handle->logger, LOGGER_SEVERITY_INFO, "Added entity \"%s\" under parent \"%s\".\n", tarasque_engine_entity_get_name(new_entity)->data, tarasque_engine_entity_get_name(full_entity)->data);

    range_destroy_dynamic(handle->alloc, &RANGE_TO_ANY(new_entity_id));

    handle->update_active_entities = true;

    return tarasque_engine_entity_get_specific_data(new_entity);
}

/**
 * @brief Queues a command to remove an entity from the game tree. All children of the entity will be also removed.
 *
 * @param[in] entity Entity to remove.
 */
void tarasque_entity_queue_remove(tarasque_entity *entity)
{
    if (!entity) {
        return;
    }

    command cmd = { 0u };
    tarasque_engine_entity *full_entity = tarasque_engine_entity_get_containing_full_entity(entity);
    tarasque_engine *handle = tarasque_engine_entity_get_host_engine_handle(full_entity);

    if (handle) {
        cmd = command_create_remove_entity(full_entity, handle->alloc);
        command_queue_append(handle->commands, cmd, handle->alloc);
    }
}

/**
 * @brief Execute a graft relative to some entity.
 * The graft may queue any kind of operations to the engine, as described by its specific documentation.
 * Grafts are usually used to add entities in bulk.
 *
 * @param[in] entity Target entity from which the graft will take place.
 * @param[in] str_id Name (copied) to give to the root of the graft.
 * @param[in] graft_data Data (copied) describing the graft and its arguments.
 */
void tarasque_entity_graft(tarasque_entity *entity, const char *str_id, tarasque_specific_graft graft_data)
{
    if (!entity || !graft_data.graft_procedure) {
        return;
    }

    graft_data.graft_procedure(entity, graft_data.args);
}

/**
 * @brief Queue a command to subscribe an entity's callback to an event.
 * If this entity is removed before the operation is done, the command is also removed.
 *
 * @param[in] entity Entity subscribing the callback.
 * @param[in] str_event_name Name (copied) of the event the entity wants to subscribe a callback to.
 * @param[in] callback Pointer to the callback that will receive the entity's data and event data.
 */
void tarasque_entity_queue_subscribe_to_event(tarasque_entity *entity,  const char *str_event_name, tarasque_specific_event_subscription subscription_data)
{
    if (!entity) {
        return;
    }

    command cmd = { 0u };
    tarasque_engine_entity *full_entity = tarasque_engine_entity_get_containing_full_entity(entity);
    tarasque_engine *handle = tarasque_engine_entity_get_host_engine_handle(full_entity);

    if (handle) {
        cmd = command_create_subscribe_to_event(full_entity, str_event_name, subscription_data, handle->alloc);
        command_queue_append(handle->commands, cmd, handle->alloc);
    }
}

/**
 * @brief Immediately stacks a named event to be sent to all entities registered to the event's name.
 *
 * @param[in] entity Entity sending the event.
 * @param[in] str_event_name Name (copied) of the event stacked.
 * @param[in] event_data Event's specific data (copied).
 */
void tarasque_entity_stack_event(tarasque_entity *entity, const char *str_event_name, tarasque_specific_event event_data)
{
    if (!entity) {
        return;
    }

    tarasque_engine_entity *full_entity = tarasque_engine_entity_get_containing_full_entity(entity);
    tarasque_engine *handle = tarasque_engine_entity_get_host_engine_handle(full_entity);

    if (event_data.is_detached) {
        event_stack_push(handle->events, handle->root_entity, str_event_name, event_data.data_size, event_data.data, handle->alloc);
    } else {
        event_stack_push(handle->events, full_entity, str_event_name, event_data.data_size, event_data.data, handle->alloc);
    }

}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Returns the first parent entity of the provided name and definition.
 * If no corresponding parent is found, the function returns NULL.
 *
 * @param[in] entity Entity from which to search for the parent.
 * @param[in] str_parent_name Name of the potential parent. Can be NULL if you just want to search by type or get the first parent.
 * @param[in] entity_def Entity definition used to create the potential parent. Can be NULL if you just want to search by name or get the first parent.
 * @return tarasque_entity *
 */
tarasque_entity *tarasque_entity_get_parent(tarasque_entity *entity, const char *str_parent_name, const tarasque_entity_definition *entity_def)
{
    if (!entity) {
        return NULL;
    }

    tarasque_engine_entity *full_entity = tarasque_engine_entity_get_containing_full_entity(entity);
    full_entity = tarasque_engine_entity_get_parent(full_entity);

    if ((entity_def == NULL) && (str_parent_name == NULL)) {
        return tarasque_engine_entity_get_specific_data(full_entity);
    }

    while ((full_entity != NULL)
            && (((entity_def == NULL) || (!tarasque_engine_entity_has_definition(full_entity, *entity_def)))
                    && ((str_parent_name == NULL) || (identifier_compare_to_cstring(tarasque_engine_entity_get_name(full_entity), str_parent_name) != 0))))
    {
        full_entity = tarasque_engine_entity_get_parent(full_entity);
    }

    return tarasque_engine_entity_get_specific_data(full_entity);
}

/**
 * @brief Returns the child of the entity at the end of the provided path. If no such child is found, returns NULL.
 * If the given path is empty, the root is returned : it is the entity given as argument.
 *
 * @param[in] entity Entity from which to search for the child.
 * @param[in] path Path to the potential child entity.
 * @param[in] entity_def Entity definition used to create the potential parent. Can be NULL if you just want to search by name..
 * @return tarasque_entity *
 */
tarasque_entity *tarasque_entity_get_child(tarasque_entity *entity, const char *str_path, const tarasque_entity_definition *entity_def)
{
    if (!entity || !str_path) {
        return NULL;
    }

    tarasque_engine_entity *full_entity = tarasque_engine_entity_get_containing_full_entity(entity);
    tarasque_engine *handle = tarasque_engine_entity_get_host_engine_handle(full_entity);
    tarasque_engine_entity *found_entity = NULL;
    path *child_path = NULL;

    if (handle) {
        child_path = path_from_cstring(str_path, handle->alloc);
        found_entity = tarasque_engine_entity_get_child(full_entity, child_path);
        path_destroy(&child_path, handle->alloc);
    }

    if (entity_def && !tarasque_engine_entity_has_definition(found_entity, *entity_def)) {
        return NULL;
    }

    return tarasque_engine_entity_get_specific_data(found_entity);
}

/**
 * @brief
 *
 * @param entity
 * @param entity_def
 * @return
 */
bool tarasque_entity_is(tarasque_entity *entity, tarasque_entity_definition entity_def)
{
    tarasque_engine_entity *full_entity = tarasque_engine_entity_get_containing_full_entity(entity);
    return tarasque_engine_entity_has_definition(full_entity, entity_def);
}

// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param handle
 * @param str_storage
 * @param str_file_path
 */
void tarasque_engine_declare_resource(tarasque_engine *handle, const char *str_storage, const char *str_file_path)
{
    if (!handle) {
        return;
    }

    resource_manager_check(handle->res_manager, str_storage, str_file_path, handle->alloc);
}

/**
 * @brief
 *
 * @param entity
 * @param str_storage
 * @param str_file_path
 * @return
 */
void *tarasque_entity_fetch_resource(tarasque_entity *entity, const char *str_storage, const char *str_file_path)
{
    if (!entity || !str_file_path) {
        return NULL;
    }

    tarasque_engine_entity *full_entity = tarasque_engine_entity_get_containing_full_entity(entity);
    tarasque_engine *handle = tarasque_engine_entity_get_host_engine_handle(full_entity);

    if (!handle) {
        return NULL;
    }

    return resource_manager_fetch(handle->res_manager, str_storage, str_file_path, handle->alloc);
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
static void tarasque_engine_annihilate_entity_and_chilren(tarasque_engine *handle, tarasque_engine_entity *target)
{
    tarasque_engine_entity_range *all_children = NULL;

    if (!handle) {
        return;
    }

    all_children = tarasque_engine_entity_get_children(target, handle->alloc);
    for (i64 i = (i64) all_children->length - 1 ; i >= 0 ; i--) {
        tarasque_engine_annihilate_entity(handle, all_children->data[i]);
    }
    range_destroy_dynamic(handle->alloc, &RANGE_TO_ANY(all_children));

    tarasque_engine_annihilate_entity(handle, target);
}

/**
 * @brief Removes an entity from the engine : removes it from the game tree and its associated commands and events.
 * The function will not touch the entity's children but will still deparent the entity.
 *
 * @param[inout] handle Engine handle.
 * @param[inout] target Entity to remove.
 */
static void tarasque_engine_annihilate_entity(tarasque_engine *handle, tarasque_engine_entity *target)
{
    if (!handle) {
        return;
    }

    tarasque_engine_entity_deparent(target);

    tarasque_engine_entity_deinit(target);
    event_stack_remove_events_of(handle->events, target, handle->alloc);
    command_queue_remove_commands_of(handle->commands, target, handle->alloc);
    event_broker_unsubscribe_from_all(handle->pub_sub, target, handle->alloc);
    tarasque_engine_entity_destroy(&target, handle->alloc);
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
    tarasque_engine_entity *subject = NULL;

    if (!handle) {
        return;
    }

    subject = cmd.source;
    if (!subject) {
        subject = handle->root_entity;
    }

    switch (cmd.flavor) {
    case COMMAND_REMOVE_ENTITY:
        tarasque_engine_process_command_remove_entity(handle, subject, &(cmd.specific.remove_entity));
        break;
    case COMMAND_SUBSCRIBE_TO_EVENT:
        tarasque_engine_process_command_subscribe_to_event(handle, &(cmd.specific.subscribe_to_event));
        break;
    default:
        break;
    }

    command_destroy(&cmd, handle->alloc);
}

/**
 * @brief Processes a command trusted to be a command to remove an entity at some position in the game tree.
 *
 * @param[inout] handle Engine handle.
 * @param[in] subject Entity that sent the command.
 * @param[in] cmd Command containing the removal data.
 */
static void tarasque_engine_process_command_remove_entity(tarasque_engine *handle, tarasque_engine_entity *subject, command_remove_entity *cmd)
{
    if (!handle || !cmd) {
        return;
    }

    if (cmd->removed == handle->root_entity) {
        logger_log(handle->logger, LOGGER_SEVERITY_ERRO, "Cannot remove root entity.\n");
        return;
    }

    logger_log(handle->logger, LOGGER_SEVERITY_INFO, "Removed entity \"%s\".\n", tarasque_engine_entity_get_name(subject)->data);
    tarasque_engine_annihilate_entity_and_chilren(handle, cmd->removed);
    handle->update_active_entities = true;
}

/**
 * @brief Processes a command trusted to be a command to subscribe an entity to an event name.
 *
 * @param[inout] handle Engine handle.
 * @param[in] subject Entity that sent the command.
 * @param[in] cmd Command containing the subscribtion data.
 */
static void tarasque_engine_process_command_subscribe_to_event(tarasque_engine *handle, command_subscribe_to_event *cmd)
{
    if (!handle) {
        return;
    }

    event_broker_subscribe(handle->pub_sub, cmd->subscribed, cmd->target_event_name, cmd->subscription_data, handle->alloc);
    logger_log(handle->logger, LOGGER_SEVERITY_INFO, "Entity \"%s\" subscribed callback %#010x to event \"%s\".\n", tarasque_engine_entity_get_name(cmd->subscribed)->data, cmd->subscription_data.callback, cmd->target_event_name->data);
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

    event_broker_publish(handle->pub_sub, processed_event);

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
    tarasque_engine_entity_range *all_entities = NULL;

    if (!handle ||!handle->active_entities) {
        return;
    }

    for (size_t i = 0u ; i < handle->active_entities->length ; i++) {
        tarasque_engine_entity_step_frame(handle->active_entities->data[i], elapsed_ms);
    }
}

/**
 * @brief Fills the internal entities buffer collection if it was marked as dirty.
 * The active entities collection is filled from parent to children from the root entity.
 *
 * @param[in] handle Traget engine instance.
 */
static void tarasque_engine_update_active_entities(tarasque_engine *handle)
{
    if (!handle || !handle->update_active_entities) {
        return;
    }

    if (handle->active_entities) {
        range_destroy_dynamic(handle->alloc, &RANGE_TO_ANY(handle->active_entities));
    }

    handle->active_entities = tarasque_engine_entity_get_children(handle->root_entity, handle->alloc);
}
