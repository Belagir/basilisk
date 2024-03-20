/**
 * @file event.h
 * @author gabriel ()
 * @brief Manipulate events and event subscriptions managers.
 * @version 0.1
 * @date 2024-03-07
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef __EVENT_H__
#define __EVENT_H__

#include "../common.h"
#include "../entity/entity.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* Opaque type to an object able to manage subscriptions to events. */
typedef struct event_broker event_broker;

/* OPaque type to a FILO collection of events. */
typedef struct event_stack event_stack;

// -------------------------------------------------------------------------------------------------

/**
 * @brief Data layout of an event sent from an entity to other entities.
 */
typedef struct event {
    /** Name of the event : this is the identifier the entities can subscribe callbacks to. */
    identifier *name;

    /** Number of bytes taken in memory by the event data. */
    size_t data_size;
    /** User-defined data the event is carrying. */
    void *data;
} event;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* Allocates a publisher / subscriber object and returns a pointer to it. */
event_broker *event_broker_create(allocator alloc);

/* Releases memory taken by a publisher / subscriber object and nullifies the pointer passed. */
void event_broker_destroy(event_broker **broker, allocator alloc);

// -------------------------------------------------------------------------------------------------

/* Allocates an event stack and returns a pointer to it. */
event_stack *event_stack_create(allocator alloc);

/* Releases memory taken by an event stack and nullifies the pointer passed. */
void event_stack_destroy(event_stack **stack, allocator alloc);

// -------------------------------------------------------------------------------------------------

/* Subscribes an event callback to an event name. Events sharing the name will be sent to the callback. */
void event_broker_subscribe(event_broker *broker, tarasque_engine_entity *subscribed, identifier *target_event_name, tarasque_event_subscription_specific_data subscription_data, allocator alloc);

/* Unsubscribes an event callback from the event broker. */
void event_broker_unsubscribe(event_broker *broker, tarasque_engine_entity *target, identifier *target_event_name, tarasque_event_subscription_specific_data subscription_data, allocator alloc);

/* Unsubscribes all callbacks associated to an entity from all events. */
void event_broker_unsubscribe_from_all(event_broker *broker, tarasque_engine_entity *target, allocator alloc);

/* Sends an event to callbacks registered to its name. */
void event_broker_publish(event_broker *broker, event ev);

// -------------------------------------------------------------------------------------------------

/* Builds and pushes an event on top of the stack. */
void event_stack_push(event_stack *stack, tarasque_engine_entity *source, const char *str_event_name, size_t event_data_size, const void *event_data, allocator alloc);

/* Pop the most recent event from the stack and returns it. */
event event_stack_pop(event_stack *stack);

/* Remove all events tied to some entity. */
void event_stack_remove_events_of(event_stack *stack, tarasque_engine_entity *source, allocator alloc);

/* Returns the number of events in the stack. */
size_t event_stack_length(const event_stack *stack);

/* Releases memory taken by an event and zeroes it out. */
void event_destroy(event *ev, allocator alloc);

#endif
