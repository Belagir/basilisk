/**
 * @file event.c
 * @author gabriel ()
 * @brief Implemetation details for the event system.
 * @version 0.1
 * @date 2024-03-08
 *
 * @copyright Copyright (c) 2024
 *
 */
#include <ustd/sorting.h>

#include "../entity/basilisk_entity.h"
#include "basilisk_event.h"
#include "event_subscription/basilisk_event_subscription.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Stores an event linked to an entity that stacked it.
 */
typedef struct event_stacked {
    /** Entity that stacked the event. Might be NULL. */
    basilisk_engine_entity *source;
    /** Actual event. */
    event ev;
} event_stacked;

// -------------------------------------------------------------------------------------------------

/**
 * @brief Maintains lists of callbacks subscribed to events.
 */
typedef struct event_broker {
    /** Collection of all lists of subscriptions. */
    RANGE(event_subscription_list) *subs;
} event_broker;

// -------------------------------------------------------------------------------------------------

/**
 * @brief Stacks events top be retreived later.
 */
typedef struct event_stack {
    /** Actual stack implementation with a range. */
    RANGE(event_stacked) *stack_impl;
} event_stack;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* Allocates an event from some user adta and a name. */
static event event_create(const char *str_event_name, size_t event_data_size, const void *event_data, allocator alloc);

/* Removes all subcriptions with zero callacks registered. */
static void event_broker_cleanup_empty_subscriptions(event_broker *broker, allocator alloc);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Allocates a new event broker to subscribe pairs of entities and callbacks to event names.
 *
 * @param[inout] alloc Allocator used for the creation.
 * @return event_broker *
 */
event_broker *event_broker_create(allocator alloc)
{
    event_broker *new_broker = NULL;

    new_broker = alloc.malloc(alloc, sizeof(*new_broker));

    if (new_broker) {
        *new_broker = (event_broker) {
                .subs = range_create_dynamic(alloc, sizeof(*new_broker->subs->data), BASILISK_COLLECTIONS_START_LENGTH),
        };
    }

    return new_broker;
}

/**
 * @brief Releases the memory taken by an event broker and nullifies the pointer passed.
 *
 * @param[inout] broker Object to destroy.
 * @param[inout] alloc Allocator used for the memory free.
 */
void event_broker_destroy(event_broker **broker, allocator alloc)
{
    if (!broker || !*broker) {
        return;
    }

    for (size_t i = 0u ; i < (*broker)->subs->length ; i++) {
        event_subscription_list_destroy((*broker)->subs->data + i, alloc);
    }

    range_destroy_dynamic(alloc, &RANGE_TO_ANY((*broker)->subs));
    alloc.free(alloc, *broker);

    *broker = NULL;
}

// -------------------------------------------------------------------------------------------------

/**
 * @brief Allocates a new event stack to store events.
 *
 * @param[inout] alloc Allocator used for the creation.
 * @return event_stack *
 */
event_stack *event_stack_create(allocator alloc)
{
    event_stack *new_stack = NULL;

    new_stack = alloc.malloc(alloc, sizeof(*new_stack));

    if (new_stack) {
        *new_stack = (event_stack) {
                .stack_impl = range_create_dynamic(alloc, sizeof(*(new_stack->stack_impl->data)), BASILISK_COLLECTIONS_START_LENGTH),
        };
    }

    return new_stack;
}

/**
 * @brief Releases the memory taken by an event stack and nullifies the pointer passed to it.
 *
 * @param[inout] stack Object destroyed.
 * @param[inout] alloc Allocator used for the free.
 */
void event_stack_destroy(event_stack **stack, allocator alloc)
{
    if (!stack || !*stack) {
        return;
    }

    for (size_t i = 0u ; i < (*stack)->stack_impl->length ; i++) {
        event_destroy(&(*stack)->stack_impl->data[i].ev, alloc);
    }

    range_destroy_dynamic(alloc, &RANGE_TO_ANY((*stack)->stack_impl));

    alloc.free(alloc, *stack);
    *stack = NULL;
}

// -------------------------------------------------------------------------------------------------

/**
 * @brief Subscribes an entity and its callback to an event name.
 * When an event of this name will be published to the broker, the callback will be executed, receiving as arguments the entity data and event data.
 *
 * @param[inout] broker Broker to store the subscription.
 * @param[in] subscribed Entity that adds the subscription.
 * @param[in] target_event_name Event the entity subscribes the callback to.
 * @param[in] subscription_data Callback data subscribed under the event.
 * @param[inout] alloc Allocator to use for eventual list creation or extension.
 */
void event_broker_subscribe(event_broker *broker, basilisk_engine_entity *subscribed, identifier *target_event_name, basilisk_specific_event_subscription subscription_data, allocator alloc)
{
    size_t list_pos = 0u;
    event_subscription_list created_list = { 0u };

    if (!broker) {
        return;
    }

    if (!sorted_range_find_in(RANGE_TO_ANY(broker->subs), &identifier_compare, &target_event_name, &list_pos)) {
        created_list = event_subscription_list_create(target_event_name, alloc);
        broker->subs = range_ensure_capacity(alloc, RANGE_TO_ANY(broker->subs), 1);
        list_pos = sorted_range_insert_in(RANGE_TO_ANY(broker->subs), &identifier_compare, &created_list);
    }

    event_subscription_list_append(broker->subs->data + list_pos, subscribed, subscription_data, alloc);
}

/**
 * @brief Removes an entity and its callback from a subscription to an event.
 *
 * @param[inout] broker Broker currently storing the subscription.
 * @param[in] target Entity that subscribed the callback.
 * @param[in] target_event_name Event the callback is subscribed to.
 * @param[in] subscription_data Callback data previously subscribed to the event.
 * @param[inout] alloc Allocator used for the eventual list deletion.
 */
void event_broker_unsubscribe(event_broker *broker, basilisk_engine_entity *target, identifier *target_event_name, basilisk_specific_event_subscription subscription_data, allocator alloc)
{
    size_t list_pos = 0u;

    if (!broker || !target || !target_event_name || !subscription_data.callback) {
        return;
    }

    if (sorted_range_find_in(RANGE_TO_ANY(broker->subs), &identifier_compare, &target_event_name, &list_pos)) {
        event_subscription_list_remove(broker->subs->data + list_pos, target, subscription_data);
    }

    event_broker_cleanup_empty_subscriptions(broker, alloc);
}

/**
 * @brief Removes all subscription that link back to some entity.
 *
 * @param[inout] broker Broker currently storing the subscriptions.
 * @param[in] target Entity that might have subscribed callbacks.
 * @param[inout] alloc Allocator used for the eventual list deletion.
 */
void event_broker_unsubscribe_from_all(event_broker *broker, basilisk_engine_entity *target, allocator alloc)
{
    size_t pos = 0u;

    if (!broker || !target) {
        return;
    }

    for (size_t i = 0u ; i < broker->subs->length ; i++) {
        event_subscription_list_remove_all_from(broker->subs->data + i, target);
    }

    event_broker_cleanup_empty_subscriptions(broker, alloc);
}

/**
 * @brief Publishes an event to all registered callbacks  that subscribed to its name.
 *
 * @param[inout] broker Target broker.
 * @param[in] ev event sent to the callbacks.
 */
void event_broker_publish(event_broker *broker, event ev)
{
    size_t list_pos = 0u;

    if (!broker) {
        return;
    }

    if (sorted_range_find_in(RANGE_TO_ANY(broker->subs), &identifier_compare, &(ev.name), &list_pos)) {
        event_subscription_list_publish(broker->subs->data + list_pos, ev);
    }
}

// -------------------------------------------------------------------------------------------------

/**
 * @brief Creates and pushes an event on top of the stack.
 *
 * @param[inout] stack Target stack to populate.
 * @param[in] source Entity adding the event.
 * @param[in] str_event_name Null-terminated string (copied) of the name of the event.
 * @param[in] event_data_size Size, in bytes, of the event's data.
 * @param[in] event_data Event's data (copied) to stack.
 * @param[inout] alloc Allocator used for the copies and eventual stack extension.
 */
void event_stack_push(event_stack *stack, basilisk_engine_entity *source, const char *str_event_name, size_t event_data_size, const void *event_data, allocator alloc)
{
    event new_event = { 0u };

    if (!stack || !source || !str_event_name) {
        return;
    }

    new_event = event_create(str_event_name, event_data_size, event_data, alloc);

    stack->stack_impl = range_ensure_capacity(alloc, RANGE_TO_ANY(stack->stack_impl), 1);
    range_insert_value(RANGE_TO_ANY(stack->stack_impl), stack->stack_impl->length, &(event_stacked) { .source = source, .ev = new_event });
}

/**
 * @brief Removes the newest event from the stack and returns it.
 *
 * @param[inout] stack Stack to pop.
 * @return event
 */
event event_stack_pop(event_stack *stack)
{
    event returned_event = { 0u };

    if (!stack || (stack->stack_impl->length == 0u)) {
        return (event) { 0u };
    }

    returned_event = stack->stack_impl->data[stack->stack_impl->length - 1u].ev;
    range_remove(RANGE_TO_ANY(stack->stack_impl), stack->stack_impl->length - 1u);

    return returned_event;
}

/**
 * @brief Removes all avents from the stack that were sent by some entity.
 *
 * @param[inout] stack Stack to modify.
 * @param[in] source Entity that might have stacked events.
 * @param[inout] alloc Allocator used to release memory taken by the events.
 */
void event_stack_remove_events_of(event_stack *stack, basilisk_engine_entity *source, allocator alloc)
{
    size_t pos = 0u;

    if (!stack || !source) {
        return;
    }

    while (pos < stack->stack_impl->length) {
        if (stack->stack_impl->data[pos].source == source) {
            event_destroy(&(stack->stack_impl->data[pos].ev), alloc);
            range_remove(RANGE_TO_ANY(stack->stack_impl), pos);
        } else {
            pos += 1u;
        }
    }
}

/**
 * @brief Returns the number of stacked events.
 *
 * @param[in] stack Examined stack.
 * @return size_t
 */
size_t event_stack_length(const event_stack *stack)
{
    if (!stack) {
        return 0u;
    }

    return stack->stack_impl->length;
}

/**
 * @brief Releases memory taken by an event and nullifies the pointer given to it.
 *
 * @param[inout] ev Target event to destroy.
 * @param[inout] alloc Allocator used for the free.
 */
void event_destroy(event *ev, allocator alloc)
{
    if (!ev) {
        return;
    }

    range_destroy_dynamic(alloc, &RANGE_TO_ANY(ev->name));

    if (ev->data) {
        alloc.free(alloc, ev->data);
    }

    *ev = (event) { 0u };
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Creates an event from user data.
 *
 * @param[in] str_event_name Name of the event.
 * @param[in] event_data_size Number of bytes taken by the event data.
 * @param[in] event_data Pointer to some foreign event data.
 * @param[inout] alloc Allocor used for the copies.
 * @return event
 */
static event event_create(const char *str_event_name, size_t event_data_size, const void *event_data, allocator alloc)
{
    event new_event = (event) {
            .name = identifier_from_cstring(str_event_name, alloc),
    };

    if (event_data && (event_data_size > 0u)) {
        new_event.data = alloc.malloc(alloc, event_data_size);
        bytewise_copy(new_event.data, event_data, event_data_size);
        new_event.data_size = event_data_size;
    }

    return new_event;
}

/**
 * @brief Removes all empty subscriptions (subscription lists with zero callbacks registered) from an event broker object.
 *
 * @param[inout] broker Target broker to clean.
 * @param[inout] alloc Allocator used to destroy the empty subscription lists.
 */
static void event_broker_cleanup_empty_subscriptions(event_broker *broker, allocator alloc)
{
    size_t pos = 0u;

    if (!broker) {
        return;
    }

    while (pos < broker->subs->length) {
        if (broker->subs->data[pos].subscription_list->length == 0u) {
            event_subscription_list_destroy(broker->subs->data + pos, alloc);
            range_remove(RANGE_TO_ANY(broker->subs), pos);
        } else {
            pos += 1u;
        }
    }
}
