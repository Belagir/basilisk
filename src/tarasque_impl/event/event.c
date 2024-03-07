
#include <ustd/sorting.h>

#include "../entity/entity.h"
#include "event.h"
#include "event_subscription/event_subscription.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

typedef struct event_stacked {
    entity *source;
    event ev;
} event_stacked;

// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 */
typedef struct event_broker {
    range(event_subscription_list) *subs;
} event_broker;

// -------------------------------------------------------------------------------------------------

typedef struct event_stack {
    range(event_stacked) *stack_impl;
} event_stack;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* */
static event event_create(const char *str_event_name, size_t event_data_size, const void *event_data, allocator alloc);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param alloc
 * @return
 */
event_broker *event_broker_create(allocator alloc)
{
    event_broker *new_broker = NULL;

    new_broker = alloc.malloc(alloc, sizeof(*new_broker));

    if (new_broker) {
        *new_broker = (event_broker) {
                .subs = range_create_dynamic(alloc, sizeof(*new_broker->subs->data), TARASQUE_COLLECTIONS_START_SIZE),
        };
    }

    return new_broker;
}

/**
 * @brief
 *
 * @param broker
 * @param alloc
 */
void event_broker_destroy(event_broker **broker, allocator alloc)
{
    if (!broker || !*broker) {
        return;
    }

    for (size_t i = 0u ; i < (*broker)->subs->length ; i++) {
        event_subscription_list_destroy((*broker)->subs->data + i, alloc);
    }

    range_destroy_dynamic(alloc, &range_to_any((*broker)->subs));
    alloc.free(alloc, *broker);

    *broker = NULL;
}

// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param alloc
 * @return
 */
event_stack *event_stack_create(allocator alloc)
{
    event_stack *new_stack = NULL;

    new_stack = alloc.malloc(alloc, sizeof(*new_stack));

    if (new_stack) {
        *new_stack = (event_stack) {
                .stack_impl = range_create_dynamic(alloc, sizeof(*(new_stack->stack_impl->data)), TARASQUE_COLLECTIONS_START_SIZE),
        };
    }

    return new_stack;
}

/**
 * @brief
 *
 * @param stack
 * @param alloc
 */
void event_stack_destroy(event_stack **stack, allocator alloc)
{
    if (!stack || !*stack) {
        return;
    }

    for (size_t i = 0u ; i < (*stack)->stack_impl->length ; i++) {
        event_destroy(&(*stack)->stack_impl->data[i].ev, alloc);
    }

    range_destroy_dynamic(alloc, &range_to_any((*stack)->stack_impl));

    alloc.free(alloc, *stack);
    *stack = NULL;
}

// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param brkr
 * @param subscribed
 * @param target_event_name
 * @param callback
 * @param alloc
 */
void event_broker_subscribe(event_broker *broker, entity *subscribed, identifier *target_event_name, void (*callback)(void *entity_data, void *event_data), allocator alloc)
{
    size_t list_pos = 0u;
    event_subscription_list created_list = { 0u };

    if (!broker) {
        return;
    }

    if (!sorted_range_find_in(range_to_any(broker->subs), &identifier_compare, &target_event_name, &list_pos)) {
        created_list = event_subscription_list_create(target_event_name, alloc);
        list_pos = sorted_range_insert_in(range_to_any(broker->subs), &identifier_compare, &created_list);
    }

    event_subscription_list_append(broker->subs->data + list_pos, subscribed, callback, alloc);
}

/**
 * @brief
 *
 * @param broker
 * @param target
 * @param target_event_name
 * @param callback
 * @param alloc
 */
void event_broker_unsubscribe(event_broker *broker, entity *target, identifier *target_event_name, void (*callback)(void *entity_data, void *event_data), allocator alloc)
{
    size_t list_pos = 0u;

    if (!broker || !target || !target_event_name || !callback) {
        return;
    }

    if (sorted_range_find_in(range_to_any(broker->subs), &identifier_compare, &target_event_name, &list_pos)) {
        event_subscription_list_remove(broker->subs->data + list_pos, target, callback);
    }
}

/**
 * @brief
 *
 * @param broker
 * @param target
 * @param alloc
 */
void event_broker_unsubscribe_from_all(event_broker *broker, entity *target, allocator alloc)
{
    if (!broker || !target) {
        return;
    }

    for (size_t i = 0u ; i < broker->subs->length ; i++) {
        event_subscription_list_remove_all_from(broker->subs->data + i, target);
    }
}

/**
 * @brief
 *
 * @param broker
 * @param ev
 */
void event_broker_publish(event_broker *broker, event ev)
{
    size_t list_pos = 0u;

    if (!broker) {
        return;
    }

    if (sorted_range_find_in(range_to_any(broker->subs), &identifier_compare, &(ev.name), &list_pos)) {
        event_subscription_list_publish(broker->subs->data + list_pos, ev);
    }
}

// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param stack
 * @param source
 * @param str_event_name
 * @param event_data_size
 * @param event_data
 * @param alloc
 */
void event_stack_push(event_stack *stack, entity *source, const char *str_event_name, size_t event_data_size, const void *event_data, allocator alloc)
{
    event new_event = { 0u };

    if (!stack || !source || !str_event_name) {
        return;
    }

    new_event = event_create(str_event_name, event_data_size, event_data, alloc);

    stack->stack_impl = range_ensure_capacity(alloc, range_to_any(stack->stack_impl), 1);
    range_insert_value(range_to_any(stack->stack_impl), stack->stack_impl->length, &(event_stacked) { .source = source, .ev = new_event });
}

/**
 * @brief
 *
 * @param stack
 * @return
 */
event event_stack_pop(event_stack *stack)
{
    event returned_event = { 0u };

    if (!stack || (stack->stack_impl->length == 0u)) {
        return (event) { 0u };
    }

    returned_event = stack->stack_impl->data[stack->stack_impl->length - 1u].ev;
    range_remove(range_to_any(stack->stack_impl), stack->stack_impl->length - 1u);

    return returned_event;
}

/**
 * @brief
 *
 * @param stack
 * @param source
 * @param alloc
 */
void event_stack_remove_events_of(event_stack *stack, entity *source, allocator alloc)
{
    size_t pos = 0u;

    if (!stack || !source) {
        return;
    }

    while (pos < stack->stack_impl->length) {
        if (stack->stack_impl->data[pos].source == source) {
            range_remove(range_to_any(stack->stack_impl), pos);
        } else {
            pos += 1u;
        }
    }
}

/**
 * @brief
 *
 * @param stack
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
 * @brief
 *
 * @param ev
 * @param alloc
 */
void event_destroy(event *ev, allocator alloc)
{
    if (!ev) {
        return;
    }

    range_destroy_dynamic(alloc, &range_to_any(ev->name));

    if (ev->data) {
        alloc.free(alloc, ev->data);
    }

    *ev = (event) { 0u };
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param source
 * @param alloc
 * @return
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
