
#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "../common.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/*  */
typedef struct entity entity;

/*  */
typedef range(entity *) entity_range;

/*  */
typedef entity_template entity_template_copy;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/*  */
entity *entity_create(const identifier *id, entity_template_copy template, allocator alloc);
/*  */
void entity_destroy(entity **target, allocator alloc);

// -------------------------------------------------------------------------------------------------

/*  */
void entity_add_child(entity *target, entity *new_child, allocator alloc);
/*  */
void entity_remove_child(entity *target, entity *removed);
/*  */
void entity_destroy_children(entity *target, allocator alloc);

// -------------------------------------------------------------------------------------------------

/*  */
entity *entity_get_child(entity *target, const path *id_path);
/*  */
entity_range *entity_get_children(entity *target, allocator alloc);

// -------------------------------------------------------------------------------------------------

void entity_step_frame(entity *target, f32 elapsed_ms, tarasque_engine *handle);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/*  */
entity_template_copy entity_template_copy_create(entity_template template, allocator alloc);
/*  */
void entity_template_copy_destroy(entity_template_copy *template, allocator alloc);

#endif
