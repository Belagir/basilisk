
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
    identifier *target_event_name;
    
    entity *subscribed;
    void (*callback)(void *entity_data, void *event_data);
} subscription;

// -------------------------------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
typedef struct event_broker {
    range(subscription *) *subs;
} event_broker;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/*  */
static subscription *subscription_create(entity *subscribed, identifier *target_event_name, void (*callback)(void *entity_data, void *event_data), allocator alloc);

/*  */
static void subscription_destroy(subscription **sub, allocator alloc);

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
 * @param brkr 
 * @param subscribed 
 * @param target_event_name 
 * @param callback 
 * @param alloc 
 */
void event_broker_subscribe(event_broker *broker, entity *subscribed, identifier *target_event_name, void (*callback)(void *entity_data, void *event_data), allocator alloc)
{
    subscription *new_sub = subscription_create(subscribed, target_event_name, callback, alloc);

    if (new_sub) {
        broker->subs = range_ensure_capacity(alloc, range_to_any(broker->subs), 1);
        sorted_range_insert_in(range_to_any(broker->subs), &identifier_compare, &new_sub);
    }
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
static subscription *subscription_create(entity *subscribed, identifier *target_event_name, void (*callback)(void *entity_data, void *event_data), allocator alloc)
{
    subscription *new_sub = NULL;

    if (!subscribed || !target_event_name || !callback) {
        return NULL;
    }

    new_sub = alloc.malloc(alloc, sizeof(*new_sub));

    if (new_sub) {
        *new_sub = (subscription) {
                .target_event_name = range_create_dynamic_from_copy_of(alloc, range_to_any(target_event_name)),
                .subscribed = subscribed,
                .callback = callback,
        };
    }

    return new_sub;
}

/**
 * @brief 
 * 
 * @param sub 
 * @param alloc 
 */
static void subscription_destroy(subscription **sub, allocator alloc)
{
    if (!sub || !*sub) {
        return;
    }

    range_destroy_dynamic(alloc, &range_to_any((*sub)->target_event_name));

    alloc.free(alloc, *sub);
    *sub = NULL;
}
