/**
 * @file command.h
 * @author gabriel ()
 * @brief Header to access an interface to manages commands carrying information to modify the engine instance's data.
 * @version 0.1
 * @date 2024-03-06
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef __COMMAND_H__
#define __COMMAND_H__

#include "../common.h"
#include "../entity/entity.h"
#include "../grafting/grafting.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* Opaque type to a FIFO collection of command objects. */
typedef struct command_queue command_queue;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Possible types of commands that exist.
 */
typedef enum command_flavor {
    COMMAND_INVALID = 0,            /// flags an error value
    COMMAND_ADD_ENTITY,             /// flags a command to add an entity
    COMMAND_REMOVE_ENTITY,          /// flags a command to remove an entity
    COMMAND_SUBSCRIBE_TO_EVENT,     /// flags a command to subscribe an entity to an event
} command_flavor;

// -------------------------------------------------------------------------------------------------

/**
 * @brief Specific data layout for a command to add an entity at some location.
 */
typedef struct command_add_entity {
    /** Range of identifiers leading to the removal location. */
    path *id_path;
    /** Name of the new identifier. */
    identifier *id;
    /** Entity user data. */
    tarasque_specific_entity_copy user_data;
} command_add_entity;

// -------------------------------------------------------------------------------------------------

/**
 * @brief Specific data layout for a command to remove an entity at some location.
 */
typedef struct command_remove_entity {
    /** Range of identifiers leading to the removal location. */
    path *id_path;
} command_remove_entity;

// -------------------------------------------------------------------------------------------------

/**
 * @brief Specific data layout for a command to subscribe an entity and a callback to an event.
 */
typedef struct command_subscribe_to_event {
    /** Name of the event subscribed. */
    identifier *target_event_name;
    /** Non-owned reference to the subscriber entity. */
    tarasque_engine_entity *subscribed;
    /** Registered callback. */
    tarasque_specific_event_subscription subscription_data;
} command_subscribe_to_event;

// -------------------------------------------------------------------------------------------------

/**
 * @brief General command queued in the engine.
 */
typedef struct command {
    /** Discrimating flag indicating the nature of the `specific` field. */
    command_flavor flavor;
    /** Entity that sent the command. Might be null. */
    tarasque_engine_entity *source;

    /** Specific section of the command. */
    union {
        /** COMMAND_ADD_ENTITY */
        command_add_entity add_entity;
        /** COMMAND_REMOVE_ENTITY */
        command_remove_entity remove_entity;
        /** COMMAND_SUBSCRIBE_TO_EVENT */
        command_subscribe_to_event subscribe_to_event;
    } specific;
} command;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* Creates a command to add an entity. */
command command_create_add_entity(tarasque_engine_entity *source, const char *id_path, const char *id, tarasque_specific_entity user_data, allocator alloc);
/* Creates a command to remove an entity. */
command command_create_remove_entity(tarasque_engine_entity *source, const char *id_path, allocator alloc);
/* Creates a command to subscribe an entity and a callback to an event. */
command command_create_subscribe_to_event(tarasque_engine_entity *source, const char *event_name, tarasque_specific_event_subscription subscription_data, allocator alloc);

// -------------------------------------------------------------------------------------------------

/* Destroys a command and releases its memory. */
void command_destroy(command *cmd, allocator alloc);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* Creates a command queue on the heap. */
command_queue *command_queue_create(allocator alloc);
/* Destroys a command queue and releases all memory used by it and its stored commands. */
void command_queue_destroy(command_queue **queue, allocator alloc);

// -------------------------------------------------------------------------------------------------

/* Adds a command to the queue. */
void command_queue_append(command_queue *queue, command cmd, allocator alloc);
/* Removes the oldest command from the queue and returns it */
command command_queue_pop_front(command_queue *queue);
/* Returns the number of currently stored commands in the queue. */
size_t command_queue_length(const command_queue *queue);

// -------------------------------------------------------------------------------------------------

/* Removes all commands tied to an entity from the queue. */
void command_queue_remove_commands_of(command_queue *queue, tarasque_engine_entity *target, allocator alloc);

#endif
