
#ifndef __ENTITY_H__
#define __ENTITY_H__

#include "../common.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/*  */
typedef struct entity entity;

/*  */
typedef entity_template entity_template_copy;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/*  */
entity *entity_create(identifier *id, entity_template_copy template, allocator alloc);
/*  */
void entity_destroy(entity **target, allocator alloc);

// -------------------------------------------------------------------------------------------------

/*  */
void entity_add_child(entity *target, entity *new_child, allocator alloc);
/*  */
void entity_remove_child(entity *target, entity *removed);

// -------------------------------------------------------------------------------------------------

/*  */
entity *entity_get_child(entity *target, identifier *id_path);
/*  */
range(entity *) *entity_get_children(entity *target, allocator alloc);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/*  */
entity_template_copy entity_template_copy_create(entity_template template, allocator alloc);
/*  */
void entity_template_copy_destroy(entity_template_copy *template, allocator alloc);

#endif
