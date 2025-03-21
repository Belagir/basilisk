/**
 * @file basilisk.c
 * @author gabriel ()
 * @brief Implementation file for the engine main header basilisk_bare.h and support header basilisk_impl.h.
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

#include <basilisk.h>

#include "../basilisk_common.h"

#include "../command/basilisk_command.h"
#include "../entity/basilisk_entity.h"
#include "../event/basilisk_event.h"
#include "../resource/basilisk_resource.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Data layout of an engine instance. Every operation possible stems from one of the objects
 * stored in this struct : this is the central data structure of the engine, from which we can navigate
 * to any engine-owned memory.
 */
typedef struct basilisk_engine {
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
    basilisk_engine_entity *root_entity;

    /** Buffer of references to entities to use for the main loop. */
    basilisk_engine_entity_range *active_entities;
    /** Flags that the active entities buffer needs to be reloaded. */
    bool update_active_entities;

    /** Flag signaling wether the engine should exit or not the main loop. */
    bool should_quit;
} basilisk_engine;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* Removes an entity from the tree, cleaning all objects referencing to it, and does so for all its children. */
static void basilisk_engine_annihilate_entity_and_chilren(basilisk_engine *handle, basilisk_engine_entity *target);
/* Removes an entity from the tree and cleaning all objects referencing to it */
static void basilisk_engine_annihilate_entity(basilisk_engine *handle, basilisk_engine_entity *target);

// -------------------------------------------------------------------------------------------------

/* Processes a general command, applying its effects and then destroying it. */
static void basilisk_engine_process_command(basilisk_engine *handle, command cmd);
/* Processes a specific command to remove an entity from the engine, changing the state of the game tree. */
static void basilisk_engine_process_command_remove_entity(basilisk_engine *handle, basilisk_engine_entity *subject, command_remove_entity *cmd);
/* Processes a specific command to subscribe an entity to an event, changing the state of the publisher / susbcriber collection. */
static void basilisk_engine_process_command_subscribe_to_event(basilisk_engine *handle, command_subscribe_to_event *cmd);

// -------------------------------------------------------------------------------------------------

/* Processes an event, passing it to registered entities and then destroying it. */
static void basilisk_engine_process_event(basilisk_engine *handle, event processed_event);

// -------------------------------------------------------------------------------------------------

/* Steps all entities forward in time with their on_frame() callback. */
static void basilisk_engine_frame_step_entities(basilisk_engine *handle, f32 elapsed_time);

// -------------------------------------------------------------------------------------------------

/* Updates the active entities buffer if needed. */
static void basilisk_engine_update_active_entities(basilisk_engine *handle);

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
 * @return basilisk_engine*
 */
basilisk_engine *basilisk_engine_create(void)
{
    allocator used_alloc = make_system_allocator();
    basilisk_engine *new_engine = nullptr;

    signal(SIGINT, &cockatrice_engine_int_handler);

    new_engine = used_alloc.malloc(used_alloc, sizeof(*new_engine));
    if (new_engine) {
        *new_engine = (basilisk_engine) {
                .alloc = used_alloc,

                .logger = logger_create(stdout, LOGGER_ON_DESTROY_DO_NOTHING),

                .commands    = command_queue_create(used_alloc),
                .events      = event_stack_create(used_alloc),
                .pub_sub     = event_broker_create(used_alloc),
                .res_manager = resource_manager_create(used_alloc),

                .root_entity = basilisk_engine_entity_create(identifier_root, (basilisk_specific_entity) { 0u }, new_engine, used_alloc),

                .active_entities = nullptr,
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
void basilisk_engine_destroy(basilisk_engine **handle)
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

    basilisk_engine_annihilate_entity_and_chilren((*handle), (*handle)->root_entity);

    logger_log((*handle)->logger, LOGGER_SEVERITY_INFO, "Engine shut down.\n");

    logger_destroy(&(*handle)->logger);

    used_alloc.free(used_alloc, *handle);
    *handle = nullptr;
}

/**
 * @brief Returns the data pointer of the root entity to be used with basilisk_entity_*() functions.
 * The pointer itself points to unallocated memory and cannot be dereferenced.
 *
 * @param[in] handle Handle to an engine instance. If nullptr, the functions returns nullptr.
 * @return basilisk_entity *
 */
basilisk_entity *basilisk_engine_root_entity(basilisk_engine *handle)
{
    if (!handle) {
        return nullptr;
    }

    return basilisk_engine_entity_get_specific_data(handle->root_entity);
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
void basilisk_engine_run(basilisk_engine *handle, int fps) {
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
            basilisk_engine_process_command(handle, command_queue_pop_front(handle->commands));
        }

        while (event_stack_length(handle->events) > 0u) {
            basilisk_engine_process_event(handle, event_stack_pop(handle->events));
        }

        basilisk_engine_update_active_entities(handle);

        basilisk_engine_frame_step_entities(handle, (f32) frame_delay);

        (void) nanosleep(&(struct timespec) { .tv_nsec = (long) (frame_delay * 1000000.f) }, nullptr);
    } while (!handle->should_quit);
}

/**
 * @brief Flags the engine to quit on the next frame.
 * The current frame will still finish before quitting.
 *
 * @param[inout] entity entity querying the end of process.
 */
void basilisk_entity_quit(basilisk_entity *entity)
{
    if (!entity) {
        return;
    }

    basilisk_engine_entity *full_entity = basilisk_engine_entity_get_containing_full_entity(entity);

    basilisk_engine_entity_get_host_engine_handle(full_entity)->should_quit = true;
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
 * @return basilisk_entity *
 */
basilisk_entity *basilisk_entity_add_child(basilisk_entity *entity, const char *str_id, basilisk_specific_entity user_data)
{
    basilisk_entity *new_entity = nullptr;
    identifier *new_entity_id = nullptr;

    basilisk_engine_entity *full_entity = basilisk_engine_entity_get_containing_full_entity(entity);
    basilisk_engine *handle = basilisk_engine_entity_get_host_engine_handle(full_entity);

    if (!full_entity || !handle) {
        return nullptr;
    }

    new_entity_id = identifier_from_cstring(str_id, handle->alloc);

    if (!new_entity_id) {
        return nullptr;
    }

    while (basilisk_engine_entity_get_direct_child(full_entity, new_entity_id) != nullptr) {
        identifier_increment(&new_entity_id, handle->alloc);
    }

    new_entity = basilisk_engine_entity_create(new_entity_id, user_data, handle, handle->alloc);
    basilisk_engine_entity_add_child(full_entity, new_entity, handle->alloc);
    basilisk_engine_entity_init(new_entity);

    logger_log(handle->logger, LOGGER_SEVERITY_INFO, "Added entity \"%s\" under parent \"%s\".\n", basilisk_engine_entity_get_name(new_entity)->data, basilisk_engine_entity_get_name(full_entity)->data);

    range_destroy_dynamic(handle->alloc, &RANGE_TO_ANY(new_entity_id));

    handle->update_active_entities = true;

    return basilisk_engine_entity_get_specific_data(new_entity);
}

/**
 * @brief Queues a command to remove an entity from the game tree. All children of the entity will be also removed.
 *
 * @param[in] entity Entity to remove.
 */
void basilisk_entity_queue_remove(basilisk_entity *entity)
{
    if (!entity) {
        return;
    }

    command cmd = { 0u };
    basilisk_engine_entity *full_entity = basilisk_engine_entity_get_containing_full_entity(entity);
    basilisk_engine *handle = basilisk_engine_entity_get_host_engine_handle(full_entity);

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
 * @param[in] graft_data Data (copied) describing the graft and its arguments.
 */
basilisk_entity *basilisk_entity_graft(basilisk_entity *entity, basilisk_specific_graft graft_data)
{
    if (!entity || !graft_data.graft_procedure) {
        return nullptr;
    }

    return graft_data.graft_procedure(entity, graft_data.args);
}

/**
 * @brief Queue a command to subscribe an entity's callback to an event.
 * If this entity is removed before the operation is done, the command is also removed.
 *
 * @param[in] entity Entity subscribing the callback.
 * @param[in] str_event_name Name (copied) of the event the entity wants to subscribe a callback to.
 * @param[in] callback Pointer to the callback that will receive the entity's data and event data.
 */
void basilisk_entity_queue_subscribe_to_event(basilisk_entity *entity,  const char *str_event_name, basilisk_specific_event_subscription subscription_data)
{
    if (!entity) {
        return;
    }

    command cmd = { 0u };
    basilisk_engine_entity *full_entity = basilisk_engine_entity_get_containing_full_entity(entity);
    basilisk_engine *handle = basilisk_engine_entity_get_host_engine_handle(full_entity);

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
void basilisk_entity_stack_event(basilisk_entity *entity, const char *str_event_name, basilisk_specific_event event_data)
{
    if (!entity) {
        return;
    }

    basilisk_engine_entity *full_entity = basilisk_engine_entity_get_containing_full_entity(entity);
    basilisk_engine *handle = basilisk_engine_entity_get_host_engine_handle(full_entity);

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
 * If no corresponding parent is found, the function returns nullptr.
 *
 * @param[in] entity Entity from which to search for the parent.
 * @param[in] str_parent_name Name of the potential parent. Can be nullptr if you just want to search by type or get the first parent.
 * @param[in] entity_def Entity definition used to create the potential parent. Can be nullptr if you just want to search by name or get the first parent.
 * @return basilisk_entity *
 */
basilisk_entity *basilisk_entity_get_parent(basilisk_entity *entity, const char *str_parent_name, const basilisk_entity_definition *entity_def)
{
    if (!entity) {
        return nullptr;
    }

    basilisk_engine_entity *full_entity = basilisk_engine_entity_get_containing_full_entity(entity);
    full_entity = basilisk_engine_entity_get_parent(full_entity);

    if ((entity_def == nullptr) && (str_parent_name == nullptr)) {
        return basilisk_engine_entity_get_specific_data(full_entity);
    }

    while ((full_entity != nullptr)
            && (((entity_def == nullptr) || (!basilisk_engine_entity_has_definition(full_entity, *entity_def)))
                    && ((str_parent_name == nullptr) || (identifier_compare_to_cstring(basilisk_engine_entity_get_name(full_entity), str_parent_name) != 0))))
    {
        full_entity = basilisk_engine_entity_get_parent(full_entity);
    }

    return basilisk_engine_entity_get_specific_data(full_entity);
}

/**
 * @brief Returns the child of the entity at the end of the provided path. If no such child is found, returns nullptr.
 * If the given path is empty, the root is returned : it is the entity given as argument.
 *
 * @param[in] entity Entity from which to search for the child.
 * @param[in] path Path to the potential child entity.
 * @param[in] entity_def Entity definition used to create the potential parent. Can be nullptr if you just want to search by name..
 * @return basilisk_entity *
 */
basilisk_entity *basilisk_entity_get_child(basilisk_entity *entity, const char *str_path, const basilisk_entity_definition *entity_def)
{
    if (!entity || !str_path) {
        return nullptr;
    }

    basilisk_engine_entity *full_entity = basilisk_engine_entity_get_containing_full_entity(entity);
    basilisk_engine *handle = basilisk_engine_entity_get_host_engine_handle(full_entity);
    basilisk_engine_entity *found_entity = nullptr;
    path *child_path = nullptr;

    if (handle) {
        child_path = path_from_cstring(str_path, handle->alloc);
        found_entity = basilisk_engine_entity_get_child(full_entity, child_path);
        path_destroy(&child_path, handle->alloc);
    }

    if (entity_def && !basilisk_engine_entity_has_definition(found_entity, *entity_def)) {
        return nullptr;
    }

    return basilisk_engine_entity_get_specific_data(found_entity);
}

/**
 * @brief
 *
 * @param entity
 * @param entity_def
 * @return
 */
bool basilisk_entity_is(const basilisk_entity *entity, basilisk_entity_definition entity_def)
{
    basilisk_engine_entity *full_entity = basilisk_engine_entity_get_containing_full_entity(entity);
    return basilisk_engine_entity_has_definition(full_entity, entity_def);
}

// -------------------------------------------------------------------------------------------------

#undef basilisk_engine_declare_resource

/**
 * @brief Declares a resource into the engine to be used later. Trying to get (with `basilisk_entity_fetch_resource()`)
 * an undeclared resource will fail regardless of its existence.
 * In nominal, development mode (with the compilation switch BASILISK_RELEASE reset), this function will search for the
 * specified resource file and append it to a storage file named after the storage name provided.
 * With BASILISK_RELEASE set, the function will only check that the resource is present in an already existing storage file
 * of the provided name.
 * If all went right, the resource will be ready to be used by entity requesting it with `basilisk_entity_fetch_resource()`.
 *
 * @param[inout] handle Handle to an engine instance.
 * @param[in] str_storage_name Name of the storage retaining the resource data.
 * @param[in] str_file_path Path to a resource file to declare.
 */
void basilisk_engine_declare_resource(basilisk_engine *handle, const char *str_storage_name, const char *str_file_path)
{
    const char *str_storage_path = str_storage_name; // for lisibility and intent

    if (!handle) {
        return;
    }

    if(resource_manager_touch(handle->res_manager, str_storage_path, str_file_path, handle->alloc)) {
        logger_log(handle->logger, LOGGER_SEVERITY_INFO, "Detected resource \"%s\" in storage \"%s\".\n", str_file_path, str_storage_path);
    } else {
        logger_log(handle->logger, LOGGER_SEVERITY_ERRO, "Failed to detect resource \"%s\" in storage \"%s\".\n", str_file_path, str_storage_path);
    }
}

#undef basilisk_entity_fetch_resource

/**
 * @brief Returns the data from a resource present in a storage. The entity will be registered as using this storage, and if it is the first
 * one to do so, all of the storage's resources will be loaded in memory. On entity removal, the entity will be also removed from the storage's
 * users, and if it was the last one, the storage will be unloaded. You can also pass nullptr to the `str_file_path` argument to notify the engine
 * that the provided entity needs to keep the storage loaded as long as it lives.
 *
 * The memory returned is owned by the engine and will follow its own lifetime.
 *
 * @param[in] entity Entity querying a resource. It will be registered as a user of the storage.
 * @param[in] str_storage_name Name of the storage containing the resource.
 * @param[in] str_file_path File path to the actual resource. Can be nullptr.
 * @param[out] out_size Outgoing number of bytes the function returned.
 * @return void *
 */
void *basilisk_entity_fetch_resource(basilisk_entity *entity, const char *str_storage_name, const char *str_file_path, unsigned long *out_size)
{
    const char *str_storage_path = str_storage_name; // for lisibility and intent

    if (!entity) {
        return nullptr;
    }

    basilisk_engine_entity *full_entity = basilisk_engine_entity_get_containing_full_entity(entity);
    basilisk_engine *handle = basilisk_engine_entity_get_host_engine_handle(full_entity);

    if (!handle) {
        return nullptr;
    }

    resource_manager_add_supplicant(handle->res_manager, str_storage_path, full_entity, handle->alloc);
    return resource_manager_fetch(handle->res_manager, str_storage_path, str_file_path, out_size);
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
static void basilisk_engine_annihilate_entity_and_chilren(basilisk_engine *handle, basilisk_engine_entity *target)
{
    basilisk_engine_entity_range *all_children = nullptr;

    if (!handle) {
        return;
    }

    all_children = basilisk_engine_entity_get_children(target, handle->alloc);
    for (i64 i = (i64) all_children->length - 1 ; i >= 0 ; i--) {
        basilisk_engine_annihilate_entity(handle, all_children->data[i]);
    }
    range_destroy_dynamic(handle->alloc, &RANGE_TO_ANY(all_children));

    basilisk_engine_annihilate_entity(handle, target);
}

/**
 * @brief Removes an entity from the engine : removes it from the game tree and its associated commands and events.
 * The function will not touch the entity's children but will still deparent the entity.
 *
 * @param[inout] handle Engine handle.
 * @param[inout] target Entity to remove.
 */
static void basilisk_engine_annihilate_entity(basilisk_engine *handle, basilisk_engine_entity *target)
{
    if (!handle) {
        return;
    }

    basilisk_engine_entity_deparent(target);

    basilisk_engine_entity_deinit(target);
    resource_manager_remove_supplicant(handle->res_manager, target, handle->alloc);
    event_stack_remove_events_of(handle->events, target, handle->alloc);
    command_queue_remove_commands_of(handle->commands, target, handle->alloc);
    event_broker_unsubscribe_from_all(handle->pub_sub, target, handle->alloc);
    basilisk_engine_entity_destroy(&target, handle->alloc);
}


// -------------------------------------------------------------------------------------------------

/**
 * @brief Processes a command to change the engine's state. The command will be destroyed.
 *
 * @param[inout] handle Engine handle.
 * @param[inout] cmd Command to process.
 */
static void basilisk_engine_process_command(basilisk_engine *handle, command cmd)
{
    basilisk_engine_entity *subject = nullptr;

    if (!handle) {
        return;
    }

    subject = cmd.source;
    if (!subject) {
        subject = handle->root_entity;
    }

    switch (cmd.flavor) {
    case COMMAND_REMOVE_ENTITY:
        basilisk_engine_process_command_remove_entity(handle, subject, &(cmd.specific.remove_entity));
        break;
    case COMMAND_SUBSCRIBE_TO_EVENT:
        basilisk_engine_process_command_subscribe_to_event(handle, &(cmd.specific.subscribe_to_event));
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
static void basilisk_engine_process_command_remove_entity(basilisk_engine *handle, basilisk_engine_entity *subject, command_remove_entity *cmd)
{
    if (!handle || !cmd) {
        return;
    }

    if (cmd->removed == handle->root_entity) {
        logger_log(handle->logger, LOGGER_SEVERITY_ERRO, "Cannot remove root entity.\n");
        return;
    }

    logger_log(handle->logger, LOGGER_SEVERITY_INFO, "Removed entity \"%s\".\n", basilisk_engine_entity_get_name(subject)->data);
    basilisk_engine_annihilate_entity_and_chilren(handle, cmd->removed);
    handle->update_active_entities = true;
}

/**
 * @brief Processes a command trusted to be a command to subscribe an entity to an event name.
 *
 * @param[inout] handle Engine handle.
 * @param[in] subject Entity that sent the command.
 * @param[in] cmd Command containing the subscribtion data.
 */
static void basilisk_engine_process_command_subscribe_to_event(basilisk_engine *handle, command_subscribe_to_event *cmd)
{
    if (!handle) {
        return;
    }

    event_broker_subscribe(handle->pub_sub, cmd->subscribed, cmd->target_event_name, cmd->subscription_data, handle->alloc);
    logger_log(handle->logger, LOGGER_SEVERITY_INFO, "Entity \"%s\" subscribed callback %#010x to event \"%s\".\n", basilisk_engine_entity_get_name(cmd->subscribed)->data, cmd->subscription_data.callback, cmd->target_event_name->data);
}

// -------------------------------------------------------------------------------------------------

/**
 * @brief Sends an event to all entities registered to the event's name. The event is destroyed.
 *
 * @param[inout] handle Engine handle.
 * @param[inout] processed_event Event sent to entities.
 */
static void basilisk_engine_process_event(basilisk_engine *handle, event processed_event)
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
static void basilisk_engine_frame_step_entities(basilisk_engine *handle, f32 elapsed_ms)
{
    basilisk_engine_entity_range *all_entities = nullptr;

    if (!handle ||!handle->active_entities) {
        return;
    }

    for (size_t i = 0u ; i < handle->active_entities->length ; i++) {
        basilisk_engine_entity_step_frame(handle->active_entities->data[i], elapsed_ms);
    }
}

/**
 * @brief Fills the internal entities buffer collection if it was marked as dirty.
 * The active entities collection is filled from parent to children from the root entity.
 *
 * @param[in] handle Traget engine instance.
 */
static void basilisk_engine_update_active_entities(basilisk_engine *handle)
{
    if (!handle || !handle->update_active_entities) {
        return;
    }

    if (handle->active_entities) {
        range_destroy_dynamic(handle->alloc, &RANGE_TO_ANY(handle->active_entities));
    }

    handle->active_entities = basilisk_engine_entity_get_children(handle->root_entity, handle->alloc);
}
