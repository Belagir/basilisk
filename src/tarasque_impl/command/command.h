
#ifndef __COMMAND_H__
#define __COMMAND_H__

#include "../common.h"
#include "../entity/entity.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

typedef struct command_queue command_queue;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

typedef enum command_flavor {
    COMMAND_INVALID,
    COMMAND_ADD_ENTITY,
    COMMAND_REMOVE_ENTITY,
    COMMAND_SUBSCRIBE_TO_EVENT,
} command_flavor;

// -------------------------------------------------------------------------------------------------

typedef struct command_remove_entity {
    range_identifier *id_path;
} command_remove_entity;

// -------------------------------------------------------------------------------------------------

typedef struct command_add_entity {
    range_identifier *id_path;
    range_identifier *id;
    entity_template_copy template;
} command_add_entity;

// -------------------------------------------------------------------------------------------------

typedef struct command_subscribe_to_event {
    range_identifier *target_event_name;
    entity *subscribed;
    void (*callback)(void *entity_data, void *event_data);
} command_subscribe_to_event;

// -------------------------------------------------------------------------------------------------

typedef struct command {
    command_flavor flavor;
    entity *source;
    union {
        command_remove_entity remove_entity;
        command_add_entity add_entity;
        command_subscribe_to_event subscribe_to_event;
    } cmd;
    
} command;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

command command_create_add_entity(entity *source, char *id_path, char *id, entity_template template, allocator alloc);
command command_create_remove_entity(entity *source, char *id_path, allocator alloc);
command command_create_subscribe_to_event(entity *source, char *id_path, allocator alloc);

// -------------------------------------------------------------------------------------------------

void command_destroy(command *cmd, allocator alloc);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

command_queue *command_queue_create(allocator alloc);
void command_queue_destroy(command_queue **queue, allocator alloc);

// -------------------------------------------------------------------------------------------------

void command_queue_append(command_queue *queue, command cmd, allocator alloc);

#endif
