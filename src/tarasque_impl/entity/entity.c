
#include "entity.h"

/**
 * @brief 
 * 
 */
typedef struct entity {
    entity *parent;
    range(entity *) *children;
    range_identifier *id;

    entity_template_copy template;
} entity;

/**
 * @brief 
 * 
 * @param id 
 * @param  
 * @param alloc 
 * @return 
 */
entity *entity_create(range_identifier *id, entity_template_copy template, allocator alloc)
{
    entity *new_entity = NULL;

    if (!id) {
        return NULL;
    }

    new_entity = alloc.malloc(alloc, sizeof(new_entity));

    if (new_entity) {
        *new_entity = (entity) {
                .parent = NULL,
                .children = range_create_dynamic(alloc, sizeof(*new_entity->children->data), TARASQUE_COLLECTIONS_START_SIZE),
                .id = range_create_dynamic_from_copy_of(alloc, range_to_any(id)),
                .template = entity_template_copy_create(template, alloc),
        };
    }

    return new_entity;
}

void entity_destroy(entity **target, allocator alloc)
{
    if (!target || !*target) {
        return;
    }

    range_destroy_dynamic(alloc, &range_to_any((*target)->children));
    range_destroy_dynamic(alloc, &range_to_any((*target)->id));
    entity_template_copy_destroy(&(*target)->template, alloc);

    alloc.free(alloc, *target);
    *target = NULL;
}


/**
 * @brief 
 * 
 * @param  
 * @param alloc 
 * @return 
 */
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

/**
 * @brief 
 * 
 * @param  
 * @param alloc 
 */
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
