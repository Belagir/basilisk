
#include "entity.h"

typedef struct entity {
    entity *parent;
    range(entity *) *children;
    range_identifier *id;

    entity_template_copy template;
} entity;

