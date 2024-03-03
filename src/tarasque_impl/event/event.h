
#ifndef __EVENT_H__
#define __EVENT_H__

#include "../common.h"

typedef struct event_broker event_broker;

// typedef struct event_stack event_stack;

event_broker *event_broker_create(allocator alloc);

void event_broker_destroy(event_broker **broker, allocator alloc);

#endif
