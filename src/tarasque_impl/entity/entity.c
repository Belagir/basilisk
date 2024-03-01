
#include "entity.h"

typedef struct entity {
    entity *parent;
    range(entity *) *children;
    range_identifier *id;

    entity_template_copy template;
} entity;

entity_template_copy entity_template_copy_create(entity_template template, allocator alloc)
{
    entity_template_copy copy = {
            .on_init = template.on_init,
            .on_deinit = template.on_deinit,
            .on_frame = template.on_frame,
    };

    if (template.data && (template.data_size > 0u)) {
        copy.data = alloc.malloc(alloc, template.data_size);
        bytewise_copy(copy.data, template.data, template.data_size);
        copy.data_size = template.data_size;
    }

    return copy;
}

void entity_template_copy_destroy(entity_template_copy *template, allocator alloc)
{
    if (!template) {
        return;
    }

    if (template->data) {
        alloc.free(alloc, template->data);
    }

    *template = (entity_template_copy) { 0u };
}
