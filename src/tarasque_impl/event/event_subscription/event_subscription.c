
#include "event_subscription.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 */
typedef struct event_subscription_list{
    identifier *event_name;

    range(struct { entity *subscribed; void (*callback)(void *entity_data, void *event_data); }) *subscription_list;
} event_subscription_list;

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
        return;
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

    range_destroy_dynamic(alloc, (*list)->event_name);
    range_destroy_dynamic(alloc, (*list)->subscription_list);

    alloc.free(alloc, *list);
    *list = NULL;
}

