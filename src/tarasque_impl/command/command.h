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
    COMMAND_INVALID,                /// flags an error value
    COMMAND_ADD_ENTITY,             /// flags a command to add an entity
    COMMAND_REMOVE_ENTITY,          /// flags a command to remove an entity
    COMMAND_SUBSCRIBE_TO_EVENT,     /// flags a command to subscribe an entity to an event
} command_flavor;

// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 */
typedef struct command_remove_entity {
    path *id_path;
} command_remove_entity;

// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 */
typedef struct command_add_entity {
    path *id_path;
    identifier *id;
    entity_core_copy template;
} command_add_entity;

// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 */
typedef struct command_subscribe_to_event {
    identifier *target_event_name;
    entity *subscribed;
    void (*callback)(void *entity_data, void *event_data);
} command_subscribe_to_event;

// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 */
typedef struct command {
    command_flavor flavor;
    entity *source;
    union {
        command_remove_entity remove_entity;
        command_add_entity add_entity;
        command_subscribe_to_event subscribe_to_event;
    } specific;

} command;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/*  */
command command_create_add_entity(entity *source, const char *id_path, const char *id, entity_core template, allocator alloc);
/*  */
command command_create_remove_entity(entity *source, const char *id_path, allocator alloc);
/*  */
command command_create_subscribe_to_event(entity *source, const char *event_name, void (*callback)(void *entity_data, void *event_data), allocator alloc);

// -------------------------------------------------------------------------------------------------

/*  */
void command_destroy(command *cmd, allocator alloc);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/*  */
command_queue *command_queue_create(allocator alloc);
/*  */
void command_queue_destroy(command_queue **queue, allocator alloc);

// -------------------------------------------------------------------------------------------------

/*  */
void command_queue_append(command_queue *queue, command cmd, allocator alloc);
/*  */
command command_queue_pop_front(command_queue *queue);
/*  */
size_t command_queue_length(const command_queue *queue);

// -------------------------------------------------------------------------------------------------

/*  */
void command_queue_remove_commands_of(command_queue *queue, entity *target, allocator alloc);

#endif
