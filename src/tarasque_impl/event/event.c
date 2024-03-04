
#include <ustd/sorting.h>

#include "../entity/entity.h"
#include "event.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 */
typedef struct subscription {
    entity *subscribed;

    identifier *target_event_name;
    void (*callback)(void *entity_data, void *event_data);
} subscription;

// -------------------------------------------------------------------------------------------------

typedef struct event_stacked {
    entity *source;
    event_copy ev;
} event_stacked;

// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 */
typedef struct event_broker {
    range(subscription) *subs;
} event_broker;

// -------------------------------------------------------------------------------------------------

typedef struct event_stack {
    range(event_stacked) *stack_impl;
} event_stack;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/*  */
static subscription subscription_create(entity *subscribed, identifier *target_event_name, void (*callback)(void *entity_data, void *event_data), allocator alloc);

/*  */
static void subscription_destroy(subscription *sub, allocator alloc);

/* */
static bool subscription_matches(subscription sub, const identifier *event_name, entity *subscribed, void (*callback)(void *entity_data, void *event_data));

// -------------------------------------------------------------------------------------------------

/* */
static event_copy event_copy_create(const event source, allocator alloc);

/* */
static void event_copy_destroy(event_copy *ev, allocator alloc);

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
        subscription_destroy((*broker)->subs->data + i, alloc);
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
        event_copy_destroy(&(*stack)->stack_impl->data[i].ev, alloc);
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
    subscription new_sub = { 0u };

    if (!broker) {
        return;
    }

    new_sub = subscription_create(subscribed, target_event_name, callback, alloc);
    broker->subs = range_ensure_capacity(alloc, range_to_any(broker->subs), 1);
    sorted_range_insert_in(range_to_any(broker->subs), &identifier_compare_tripleref, &new_sub);
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
    size_t target_index = 0u;

    if (!broker || !target) {
        return;
    }

    if (!sorted_range_find_in(range_to_any(broker->subs), &identifier_compare_tripleref, &target, &target_index)) {
        return;
    }

    while ((target_index < broker->subs->length) && (broker->subs->data[target_index].subscribed == target)) {

        if (subscription_matches(broker->subs->data[target_index], target_event_name, target, callback)) {
            subscription_destroy(broker->subs->data + target_index, alloc);
            range_remove(range_to_any(broker->subs), target_index);
        } else {
            target_index += 1u;
        }
    }
}

/**
 * @brief
 *
 * @param stack
 * @param source
 * @param ev
 * @param alloc
 */
void event_stack_push(event_stack *stack, entity *source, event ev, allocator alloc)
{
    event_copy copy = { 0u };

    if (!stack || !source || !ev.name) {
        return;
    }

    copy = event_copy_create(ev, alloc);
    stack->stack_impl = range_ensure_capacity(alloc, range_to_any(stack->stack_impl), 1);
    range_insert_value(range_to_any(stack->stack_impl), stack->stack_impl->length, &copy);
}

/**
 * @brief
 *
 * @param stack
 * @return
 */
event_copy event_stack_pop(event_stack *stack)
{
    event_copy returned_event = { 0u };

    if (!stack || (stack->stack_impl->length == 0u)) {
        return (event_copy) { 0u };
    }

    returned_event = stack->stack_impl->data[stack->stack_impl->length - 1u].ev;
    range_remove(range_to_any(stack->stack_impl), stack->stack_impl->length - 1u);

    return returned_event;
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param subscribed
 * @param target_event_name
 * @param callback
 * @param alloc
 * @return
 */
static subscription subscription_create(entity *subscribed, identifier *target_event_name, void (*callback)(void *entity_data, void *event_data), allocator alloc)
{
    subscription new_sub = { 0u };

    if (!subscribed || !target_event_name || !callback) {
        return (subscription) { 0u };
    }

    new_sub = (subscription) {
            .target_event_name = range_create_dynamic_from_copy_of(alloc, range_to_any(target_event_name)),
            .subscribed = subscribed,
            .callback = callback,
    };

    return new_sub;
}

/**
 * @brief
 *
 * @param sub
 * @param alloc
 */
static void subscription_destroy(subscription *sub, allocator alloc)
{
    if (!sub) {
        return;
    }

    range_destroy_dynamic(alloc, &range_to_any(sub->target_event_name));
    *sub = (subscription) { 0u };
}

/**
 * @brief
 *
 * @param sub
 * @param event_name
 * @param subscribed
 * @param callback
 * @return
 */
static bool subscription_matches(subscription sub, const identifier *event_name, entity *subscribed, void (*callback)(void *entity_data, void *event_data))
{
    bool matches_name     = false;
    bool matches_entity   = false;
    bool matches_callback = false;

    matches_name     = (!event_name) || (identifier_compare(event_name, sub.target_event_name) == 0);
    matches_entity   = (!subscribed) || (subscribed == sub.subscribed);
    matches_callback = (!callback) || (callback == sub.callback);

    return matches_name && matches_entity && matches_callback;
}

/**
 * @brief
 *
 * @param source
 * @param alloc
 * @return
 */
static event_copy event_copy_create(const event source, allocator alloc)
{
    event_copy copy = (event_copy) {
            .name = range_create_dynamic_from_copy_of(alloc, range_to_any(source.name)),
    };

    if (source.data && (source.data_size > 0u)) {
        copy.data = alloc.malloc(alloc, source.data_size);
        bytewise_copy(copy.data, source.data, source.data_size);
        copy.data_size = source.data_size;
    }

    return copy;
}

/**
 * @brief
 *
 * @param ev
 * @param alloc
 */
static void event_copy_destroy(event_copy *ev, allocator alloc)
{
    if (!ev) {
        return;
    }

    if (ev->data) {
        alloc.free(alloc, ev->data);
    }

    *ev = (event_copy) { 0u };
}
