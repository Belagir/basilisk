
#include "event_subscription.h"

#include <ustd/sorting.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static i32 event_subscription_compare(const void *lhs, const void *rhs);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param event_name
 * @param alloc
 * @return
 */
event_subscription_list event_subscription_list_create(identifier *event_name, allocator alloc)
{
    event_subscription_list new_list = { 0u };

    if (!event_name) {
        return (event_subscription_list) { 0u };
    }

    new_list = (event_subscription_list) {
            .event_name = range_create_dynamic_from_copy_of(alloc, range_to_any(event_name)),
            .subscription_list = range_create_dynamic(alloc, sizeof(*(new_list.subscription_list->data)), TARASQUE_COLLECTIONS_START_SIZE),
    };

    return new_list;
}

/**
 * @brief
 *
 * @param list
 * @param alloc
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
 * @brief
 *
 * @param list
 * @param subscribed
 * @param callback
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
 * @brief
 *
 * @param list
 * @param subscribed
 * @param callback
 */
void event_subscription_list_remove(event_subscription_list *list, entity *subscribed, void (*callback)(void *entity_data, void *event_data))
{
    if (!list || !subscribed) {
        return;
    }

    sorted_range_remove_from(range_to_any(list->subscription_list), &event_subscription_compare, &(event_subscription) { .subscribed = subscribed, .callback = callback, });
}

// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param list
 * @param ev
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

// -------------------------------------------------------------------------------------------------
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
