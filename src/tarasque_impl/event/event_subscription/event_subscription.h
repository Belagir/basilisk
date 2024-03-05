
#ifndef __EVENT_SUBSCRIPTION_H__
#define __EVENT_SUBSCRIPTION_H__

#include "../../common.h"
#include "../../event/event.h"
#include "../../entity/entity.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 */
typedef struct event_subscription { entity *subscribed; void (*callback)(void *entity_data, void *event_data); } event_subscription;

/**
 * @brief
 *
 */
typedef struct event_subscription_list{
    identifier *event_name;

    range(event_subscription) *subscription_list;
} event_subscription_list;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/*  */
event_subscription_list event_subscription_list_create(identifier *event_name, allocator alloc);

/*  */
void event_subscription_list_destroy(event_subscription_list *list, allocator alloc);

// -------------------------------------------------------------------------------------------------

/*  */
void event_subscription_list_append(event_subscription_list *list, entity *subscribed, void (*callback)(void *entity_data, void *event_data));

/*  */
void event_subscription_list_remove(event_subscription_list *list, entity *subscribed, void (*callback)(void *entity_data, void *event_data));

// -------------------------------------------------------------------------------------------------

/*  */
void event_subscription_list_publish(event_subscription_list *list, event *ev);

/*  */
size_t event_subscription_list_length(const event_subscription_list *list);

#endif
