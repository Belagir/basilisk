/**
 * @file event_subscription.h
 * @author gabriel ()
 * @brief Manipulate subscriptions to events and lists of subscriptions.
 * @version 0.1
 * @date 2024-03-07
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef __EVENT_SUBSCRIPTION_H__
#define __EVENT_SUBSCRIPTION_H__

#include "../../common.h"
#include "../../event/event.h"
#include "../../entity/entity.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Store an entity-related callback to an event.
 */
typedef struct event_subscription {
    /** Reference to an entity thye callback is linked to */
    entity *subscribed;
    /** Pointer to some callback function to execute code on an event reception. */
    void (*callback)(void *entity_data, void *event_data);
} event_subscription;

/**
 * @brief Associate a list of callbacks to an event name.
 */
typedef struct event_subscription_list{
    /** Name of the event the callback list is mapped to. */
    identifier *event_name;

    /** All existing callbacks associated to the event name. */
    range(event_subscription) *subscription_list;
} event_subscription_list;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* Creates a subscription list associated to an event name. */
event_subscription_list event_subscription_list_create(identifier *event_name, allocator alloc);

/* destroys and release memory held by an event list. */
void event_subscription_list_destroy(event_subscription_list *list, allocator alloc);

// -------------------------------------------------------------------------------------------------

/* Inserts a new entry in the callback list. */
void event_subscription_list_append(event_subscription_list *list, entity *subscribed, void (*callback)(void *entity_data, void *event_data), allocator alloc);

/* Removes an entry from the callback list. */
void event_subscription_list_remove(event_subscription_list *list, entity *subscribed, void (*callback)(void *entity_data, void *event_data));

/* Removes all entries tied to some entity. */
void event_subscription_list_remove_all_from(event_subscription_list *list, entity *subscribed);

// -------------------------------------------------------------------------------------------------

/* Publishes an event to a callback list. The event is trusted to be of the right name as the one of the list. */
void event_subscription_list_publish(event_subscription_list *list, event ev);

/* Returns the number of callbacks in a list. */
size_t event_subscription_list_length(const event_subscription_list *list);

#endif
