
#include <ustd/sorting.h>

#include "entity.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief 
 * 
 */
typedef struct entity {
    entity *parent;
    range(entity *) *children;
    identifier *id;

    entity_template_copy template;
} entity;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief 
 * 
 * @param id 
 * @param  
 * @param alloc 
 * @return 
 */
entity *entity_create(identifier *id, entity_template_copy template, allocator alloc)
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

/**
 * @brief 
 * 
 * @param target 
 * @param alloc 
 */
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

// -------------------------------------------------------------------------------------------------

/**
 * @brief 
 * 
 * @param target 
 * @param new_child 
 * @param alloc 
 */
void entity_add_child(entity *target, entity *new_child, allocator alloc)
{
    if (!target || !new_child) {
        return;
    }

    target->children = range_ensure_capacity(alloc, range_to_any(target->children));
    (void) sorted_range_insert_in(range_to_any(target->children), &identifier_compare, &new_child);
}

/**
 * @brief 
 * 
 * @param target 
 * @param removed 
 */
void entity_remove_child(entity *target, entity *removed)
{
    if (!target || !removed) {
        return;
    }

    (void) sorted_range_remove_from(range_to_any(target->children), &identifier_compare, &removed);
}

/**
 * @brief 
 * 
 * @param target 
 * @param id_path 
 * @return 
 */
entity *entity_get_child(entity *target, path *id_path)
{
    if (!target) {
        return NULL;
    } else if (!id_path) {
        return target;
    }

    // TODO
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

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
