/**
 * @file event_subscription.c
 * @author gabriel ()
 * @brief Implementation file related to all that touches to event callback lists.
 * @version 0.1
 * @date 2024-03-07
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "event_subscription.h"

#include <ustd/sorting.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* Compares two event subscriptions to order the entries in the list. */
static i32 event_subscription_compare(const void *lhs, const void *rhs);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Creates a new list of entity/callback pairs tied to an event name.
 *
 * @param[in] event_name Name (copied) of the event the callbacks should receive.
 * @param[inout] alloc Allocator used for the creation of the list.
 * @return event_subscription_list
 */
event_subscription_list event_subscription_list_create(identifier *event_name, allocator alloc)
{
    event_subscription_list new_list = { 0u };

    if (!event_name) {
        return (event_subscription_list) { 0u };
    }

    new_list = (event_subscription_list) {
            .event_name = range_create_dynamic_from_copy_of(alloc, range_to_any(event_name)),
            .subscription_list = range_create_dynamic(alloc, sizeof(*(new_list.subscription_list->data)), TARASQUE_COLLECTIONS_START_LENGTH),
    };

    return new_list;
}

/**
 * @brief Destroys a list of subscriptions and all its contents, zero-ing out the contents of the struct.
 *
 * @param[inout] list List to remove from memory.
 * @param[inout] alloc Allocator used to release the memory.
 */
void event_subscription_list_destroy(event_subscription_list *list, allocator alloc)
{
    if (!list) {
        return;
    }

    range_destroy_dynamic(alloc, &range_to_any(list->event_name));
    range_destroy_dynamic(alloc, &range_to_any(list->subscription_list));

    *list = (event_subscription_list) { 0u };
}

// -------------------------------------------------------------------------------------------------

/**
 * @brief Inserts a entity / callback pair in the list to receive events under thje list's event name.
 *
 * @param[inout] list List receiving the new pair.
 * @param[in] subscribed Entity subscribing a callback to the event.
 * @param[in] callback Function called to receive the event.
 * @param[inout] alloc Allocator used to eventually extend the list.
 */
void event_subscription_list_append(event_subscription_list *list, entity *subscribed, void (*callback)(void *entity_data, void *event_data), allocator alloc)
{
    if (!list || !subscribed || !callback) {
        return;
    }

    list->subscription_list = range_ensure_capacity(alloc, range_to_any(list->subscription_list), 1);
    sorted_range_insert_in(range_to_any(list->subscription_list), &event_subscription_compare, &(event_subscription) { .subscribed = subscribed, .callback = callback, });
}

/**
 * @brief Removes an entry from the list.
 *
 * @param[inout] list List containing the element to remove.
 * @param[in] subscribed Entity that subscribed a callback.
 * @param[in] callback Callback previously registered.
 */
void event_subscription_list_remove(event_subscription_list *list, entity *subscribed, void (*callback)(void *entity_data, void *event_data))
{
    if (!list || !subscribed) {
        return;
    }

    sorted_range_remove_from(range_to_any(list->subscription_list), &event_subscription_compare, &(event_subscription) { .subscribed = subscribed, .callback = callback, });
}

/**
 * @brief Removes all entries that are linked to an entity.
 *
 * @param[inout] list List containing the elements to remove.
 * @param[in] subscribed Entity that subscribed a (some) callback(s).
 */
void event_subscription_list_remove_all_from(event_subscription_list *list, entity *subscribed)
{
    size_t subs_index = 0u;

    if (!list || !subscribed) {
        return;
    }

    if (!sorted_range_find_in(range_to_any(list->subscription_list), &event_subscription_compare, &(event_subscription) { .subscribed = subscribed, .callback = NULL, }, &subs_index)) {
        return;
    }

    while ((subs_index < list->subscription_list->length) && (list->subscription_list->data[subs_index].subscribed == subscribed)) {
        range_remove(range_to_any(list->subscription_list), subs_index);
    }
}

// -------------------------------------------------------------------------------------------------

/**
 * @brief Sends an event to the list. The name of the event is not checked to match the one expected by the list.
 * All callbacks of the list are called, receiving their entity data and the event data.
 *
 * @param[in] list List containing the callbacks.
 * @param[inout] ev Event sent to the list.
 */
void event_subscription_list_publish(event_subscription_list *list, event ev)
{
    event_subscription tmp_sub = { 0u };

    if (!list) {
        return;
    }

    for (size_t i = 0u ; i < list->subscription_list->length ; i++) {
        tmp_sub = list->subscription_list->data[i];
        if (tmp_sub.callback) {
            entity_send_event(tmp_sub.subscribed, tmp_sub.callback, ev.data);
        }
    }
}

/**
 * @brief Returns the number of callbacks in the list.
 *
 * @param[in] list Examined list.
 * @return size_t
 */
size_t event_subscription_list_length(const event_subscription_list *list)
{
    if (!list) {
        return 0u;
    }

    return list->subscription_list->length;
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static i32 event_subscription_compare(const void *lhs, const void *rhs)
{
    i32 event_name_cmp = 0;

    event_subscription *sub_lhs = (event_subscription *) lhs;
    event_subscription *sub_rhs = (event_subscription *) rhs;

    event_name_cmp = identifier_compare_tripleref(sub_lhs, sub_rhs);

    if ((event_name_cmp == 0) && sub_lhs->callback && sub_rhs->callback) {
        return (((uintptr_t) sub_lhs->callback) > ((uintptr_t) sub_rhs->callback)) - (((uintptr_t) sub_lhs->callback) < ((uintptr_t) sub_rhs->callback));
    }
    return event_name_cmp;
}
