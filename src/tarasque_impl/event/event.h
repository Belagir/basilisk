
#ifndef __EVENT_H__
#define __EVENT_H__

#include "../common.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/*  */
typedef struct event_broker event_broker;

/*  */
typedef struct event_stack event_stack;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/*  */
event_broker *event_broker_create(allocator alloc);

/*  */
void event_broker_destroy(event_broker **broker, allocator alloc);

// -------------------------------------------------------------------------------------------------

/*  */
event_stack *event_stack_create(allocator alloc);

/*  */
void event_stack_destroy(event_stack **stack, allocator alloc);

// -------------------------------------------------------------------------------------------------

/*  */
void event_broker_subscribe(event_broker *broker, entity *subscribed, identifier *target_event_name, void (*callback)(void *entity_data, void *event_data), allocator alloc);

/*  */
void event_broker_unsubscribe(event_broker *broker, entity *target, identifier *target_event_name, void (*callback)(void *entity_data, void *event_data), allocator alloc);

#endif
