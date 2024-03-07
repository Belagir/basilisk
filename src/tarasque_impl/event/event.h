
#ifndef __EVENT_H__
#define __EVENT_H__

#include "../common.h"
#include "../entity/entity.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/*  */
typedef struct event_broker event_broker;

/*  */
typedef struct event_stack event_stack;

// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 */
typedef struct event {
    identifier *name;

    size_t data_size;
    void *data;
} event;

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

/* */
void event_broker_unsubscribe_from_all(event_broker *broker, entity *target, allocator alloc);

/*  */
void event_broker_publish(event_broker *broker, event ev);

// -------------------------------------------------------------------------------------------------

/*  */
void event_stack_push(event_stack *stack, entity *source, const char *str_event_name, size_t event_data_size, const void *event_data, allocator alloc);

/*  */
event event_stack_pop(event_stack *stack);

/*  */
void event_stack_remove_events_of(event_stack *stack, entity *source, allocator alloc);

/*  */
size_t event_stack_length(const event_stack *stack);

/* */
void event_destroy(event *ev, allocator alloc);

#endif
