
#include "event_subscription.h"

#include <ustd/sorting.h>

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
event_subscription_list *event_subscription_list_create(identifier *event_name, allocator alloc)
{
    event_subscription_list *new_list = NULL;

    if (!event_name) {
        return NULL;
    }

    new_list = alloc.malloc(alloc, sizeof(*new_list));

    if (new_list) {
        *new_list = (event_subscription_list) {
            .event_name = range_create_dynamic_from_copy_of(alloc, range_to_any(event_name)),
            .subscription_list = range_create_dynamic(alloc, sizeof(*(new_list->subscription_list->data)), TARASQUE_COLLECTIONS_START_SIZE),
        };
    }

    return new_list;
}

/**
 * @brief
 *
 * @param list
 * @param alloc
 */
void event_subscription_list_destroy(event_subscription_list **list, allocator alloc)
{
    if (!list || !*list) {
        return;
    }

    range_destroy_dynamic(alloc, &range_to_any((*list)->event_name));
    range_destroy_dynamic(alloc, &range_to_any((*list)->subscription_list));

    alloc.free(alloc, *list);
    *list = NULL;
}

// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param list
 * @param subscribed
 * @param callback
 */
void event_subscription_list_append(event_subscription_list *list, entity *subscribed, void (*callback)(void *entity_data, void *event_data))
{
    if (!list || !subscribed || !callback) {
        return;
    }

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
