
#include <ustd/range.h>

#include "../entity/entity.h"
#include "event.h"

/**
 * @brief 
 * 
 */
typedef struct subscription {
    identifier *target_event_name;
    
    entity *subscribed;
    void (*callback)(void *entity_data, void *event_data);
} subscription;

/**
 * @brief 
 * 
 */
typedef struct event_broker {
    range(subscription) *subs;
} event_broker;

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

    range_destroy_dynamic(alloc, &range_to_any((*broker)->subs));
    alloc.free(alloc, *broker);

    *broker = NULL;
}