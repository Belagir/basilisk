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

#include <tarasque.h>

#include "../common.h"

#include "../command/command.h"
#include "../entity/entity.h"
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
    tarasque_engine_entity *root_entity;

    /** Flag signaling wether the engine should exit or not the main loop. */
    bool should_quit;

    /** Allocator object used for all memory operations done by the engine instance. */
    allocator alloc;
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
/* Processes a specific command to add an entity in the engine, changing the state of the game tree. */
static void tarasque_engine_process_command_add_entity(tarasque_engine *handle, tarasque_engine_entity *subject, command_add_entity cmd);

static void tarasque_engine_process_command_graft(tarasque_engine *handle, tarasque_engine_entity *subject, command_graft cmd);
/* Processes a specific command to remove an entity from the engine, changing the state of the game tree. */
static void tarasque_engine_process_command_remove_entity(tarasque_engine *handle, tarasque_engine_entity *subject, command_remove_entity cmd);
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

                .root_entity = tarasque_engine_entity_create(identifier_root, (tarasque_specific_entity) { 0u }, new_engine, used_alloc),

                .should_quit = false,

                .alloc = used_alloc,
        };

        logger_log(new_engine->logger, LOGGER_SEVERITY_INFO, "Engine is ready.\n");
    }

    signal(SIGINT, &cockatrice_engine_int_handler);

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
 * @brief Queues a command to add an entity into the game tree.
 * The path is relative to the entity passed as argument. If this entity is removed before the operation is done, the
 * command is also removed.
 *
 * @param[in] entity Target to-be-parent entity.
 * @param[in] str_path String (copied) describing a '/'-delimited path to an entity that will be the direct parent of the new entity.
 *  The path "" represents the entity passed as argument.
 * @param[in] str_id New entity's name (copied), must be unique in respect to its siblings and not contain the character '/'.
 * @param[in] user_data Basic entity information (copied).
 */
void tarasque_entity_add_child(tarasque_entity *entity, const char *str_path, const char *str_id, tarasque_specific_entity user_data)
{
    if (!entity) {
        return;
    }

    command cmd = { 0u };
    tarasque_engine_entity *full_entity = tarasque_engine_entity_get_containing_full_entity(entity);
    tarasque_engine *handle = tarasque_engine_entity_get_host_engine_handle(full_entity);

    if (handle) {
        cmd = command_create_add_entity(full_entity, str_path, str_id, user_data, handle->alloc);
        command_queue_append(handle->commands, cmd, handle->alloc);
    }
}

/**
 * @brief Queue a command to remove an entity into the game tree.
 * The path is relative to the entity passed as argument. If this entity is removed before the operation is done, the
 * command is also removed.
 *
 * @param[in] entity Target entity to be removed. The path "" represents the entity passed as argument.
 * @param[in] str_path string (copied) describing a '/'-delimited path to the removeed entity.
 */
void tarasque_entity_remove_child(tarasque_entity *entity, const char *str_path)
{
    if (!entity) {
        return;
    }

    command cmd = { 0u };
    tarasque_engine_entity *full_entity = tarasque_engine_entity_get_containing_full_entity(entity);
    tarasque_engine *handle = tarasque_engine_entity_get_host_engine_handle(full_entity);

    if (handle) {
        cmd = command_create_remove_entity(full_entity, str_path, handle->alloc);
        command_queue_append(handle->commands, cmd, handle->alloc);
    }
}

/**
 * @brief Execute a graft relative to some entity.
 * The graft may queue any kind of operations to the engine, as described by its specific documentation.
 * Grafts are usually used to add entities in bulk.
 *
 * @param[in] entity Target entity from which the graft will take place.
 * @param[in] str_path Path (copied) from which the graft is executed.
 * @param[in] str_id Name (copied) to give to the root of the graft.
 * @param[in] graft_data Data (copied) describing the graft and its arguments.
 */
void tarasque_entity_graft(tarasque_entity *entity, const char *str_path, const char *str_id, tarasque_specific_graft graft_data)
{
    if (!entity) {
        return;
    }

    command cmd = { 0u };
    tarasque_engine_entity *full_entity = tarasque_engine_entity_get_containing_full_entity(entity);
    tarasque_engine *handle = tarasque_engine_entity_get_host_engine_handle(full_entity);

    if (handle) {
        cmd = command_create_graft(full_entity, str_path, str_id, graft_data, handle->alloc);
        command_queue_append(handle->commands, cmd, handle->alloc);
    }
}

/**
 * @brief Queue a command to subscribe an entity's callback to an event.
 * If this entity is removed before the operation is done, the command is also removed.
 *
 * @param[in] entity Entity subscribing the callback.
 * @param[in] str_event_name Name (copied) of the event the entity wants to subscribe a callback to.
 * @param[in] callback Pointer to the callback that will receive the entity's data and event data.
 */
void tarasque_entity_subscribe_to_event(tarasque_entity *entity,  const char *str_event_name, tarasque_specific_event_subscription subscription_data)
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
 * @brief Completely removes an entity and its children from everything in the engine.
 * Additionally of removing the entities from the tree, it will remove all relevant events, subscriptions
 * and commands linked to the entities. All of the entities memory is released.
 *
 * @param[inout] handle Engine handle.
 * @param[inout] target Entity to remove along its children.
 */
static void tarasque_engine_annihilate_entity_and_chilren(tarasque_engine *handle, tarasque_engine_entity *target)
{
    tarasque_entity_range *all_children = NULL;

    if (!handle) {
        return;
    }

    all_children = tarasque_engine_entity_get_children(target, handle->alloc);
    for (size_t i = 0u ; i < all_children->length ; i++) {
        tarasque_engine_annihilate_entity(handle, all_children->data[i]);
    }
    range_destroy_dynamic(handle->alloc, &range_to_any(all_children));

    tarasque_engine_entity_deparent(target);
    tarasque_engine_annihilate_entity(handle, target);
}

static void tarasque_engine_annihilate_entity(tarasque_engine *handle, tarasque_engine_entity *target)
{
    if (!handle) {
        return;
    }

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
static void tarasque_engine_process_command_add_entity(tarasque_engine *handle, tarasque_engine_entity *subject, command_add_entity cmd)
{
    tarasque_engine_entity *new_entity = NULL;
    tarasque_engine_entity *found_parent = NULL;

    if (!handle) {
        return;
    }

    found_parent = tarasque_engine_entity_get_child(subject, cmd.id_path);

    if (!found_parent) {
        logger_log(handle->logger, LOGGER_SEVERITY_ERRO, "Could not find parent to add entity \"%s\".\n", cmd.id->data);
        return;

    } else if (tarasque_engine_entity_get_direct_child(found_parent, cmd.id) != NULL) {
        logger_log(handle->logger, LOGGER_SEVERITY_ERRO, "Parent \"%s\" already has child named \"%s\".\n", tarasque_engine_entity_get_name(found_parent)->data, cmd.id->data);
        return;
    }

    new_entity = tarasque_engine_entity_create(cmd.id, cmd.user_data, handle, handle->alloc);
    tarasque_engine_entity_add_child(found_parent, new_entity, handle->alloc);
    tarasque_engine_entity_init(new_entity);

    logger_log(handle->logger, LOGGER_SEVERITY_INFO, "Added entity \"%s\" under parent \"%s\".\n", tarasque_engine_entity_get_name(new_entity)->data, tarasque_engine_entity_get_name(found_parent)->data);
}

/**
 * @brief
 *
 * @param handle
 * @param subject
 * @param cmd
 */
static void tarasque_engine_process_command_graft(tarasque_engine *handle, tarasque_engine_entity *subject, command_graft cmd)
{
    tarasque_engine_entity *graft_parent = NULL;
    tarasque_engine_entity *graft_root = NULL;

    if (!handle) {
        return;
    }

    graft_parent = tarasque_engine_entity_get_child(subject, cmd.id_path);

    if (!graft_parent) {
        return;
    }

    graft_root = tarasque_engine_entity_create(cmd.id, (tarasque_entity_specific_data_copy) { 0u }, handle, handle->alloc);
    tarasque_engine_entity_add_child(graft_parent, graft_root, handle->alloc);
    tarasque_engine_entity_init(graft_parent);

    cmd.graft_data.graft_procedure(tarasque_engine_entity_get_specific_data(graft_root), cmd.graft_data.args);

    logger_log(handle->logger, LOGGER_SEVERITY_INFO, "Added graft \"%s\" under parent \"%s\".\n", cmd.id->data, tarasque_engine_entity_get_name(graft_parent)->data);
}

/**
 * @brief Processes a command trusted to be a command to remove an entity at some position in the game tree.
 *
 * @param[inout] handle Engine handle.
 * @param[in] subject Entity that sent the command.
 * @param[in] cmd Command containing the removal data.
 */
static void tarasque_engine_process_command_remove_entity(tarasque_engine *handle, tarasque_engine_entity *subject, command_remove_entity cmd)
{
    tarasque_engine_entity *found_entity = NULL;

    if (!handle) {
        return;
    }

    found_entity = tarasque_engine_entity_get_child(subject, cmd.id_path);

    if (found_entity == handle->root_entity) {
        logger_log(handle->logger, LOGGER_SEVERITY_ERRO, "Cannot remove root entity.\n");
        return;
    }

    if (!found_entity) {
        logger_log(handle->logger, LOGGER_SEVERITY_WARN, "Could not find entity \"%s\" to remove.\n", tarasque_engine_entity_get_name(subject)->data);
        return;
    }

    logger_log(handle->logger, LOGGER_SEVERITY_INFO, "Removed entity \"%s\".\n", tarasque_engine_entity_get_name(subject)->data);
    tarasque_engine_annihilate_entity_and_chilren(handle, found_entity);
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
    logger_log(handle->logger, LOGGER_SEVERITY_INFO, "Entity \"%s\" subscribed callback %#010x to event \"%s\".\n", tarasque_engine_entity_get_name(cmd.subscribed)->data, cmd.subscription_data, cmd.target_event_name->data);
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
    tarasque_entity_range *all_entities = NULL;

    if (!handle) {
        return;
    }

    // TODO (low priority because of possible big impact on implementation) : children collections should be updated on entities change, not every frame
    all_entities = tarasque_engine_entity_get_children(handle->root_entity, handle->alloc);

    if (!all_entities) {
        return;
    }

    for (size_t i = 0u ; i < all_entities->length ; i++) {
        tarasque_engine_entity_step_frame(all_entities->data[i], elapsed_ms);
    }

    range_destroy_dynamic(handle->alloc, &range_to_any(all_entities));
}
