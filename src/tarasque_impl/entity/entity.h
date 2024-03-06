
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
typedef entity_core entity_core_copy;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/*  */
entity *entity_create(const identifier *id, entity_core_copy template, tarasque_engine *handle, allocator alloc);
/*  */
void entity_destroy(entity **target, tarasque_engine *handle, allocator alloc);

// -------------------------------------------------------------------------------------------------

/*  */
void entity_add_child(entity *target, entity *new_child, allocator alloc);
/*  */
void entity_deparent(entity *target);
/*  */
void entity_destroy_children(entity *target, tarasque_engine *handle, allocator alloc);

// -------------------------------------------------------------------------------------------------

/*  */
entity *entity_get_child(entity *target, const path *id_path);
/*  */
entity_range *entity_get_children(entity *target, allocator alloc);

// -------------------------------------------------------------------------------------------------

/*  */
void entity_step_frame(entity *target, f32 elapsed_ms, tarasque_engine *handle);

/*  */
void entity_send_event(entity *target, void (*callback)(void *entity_data, void *event_data), void *event_data);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/*  */
entity_core_copy entity_template_copy_create(entity_core template, allocator alloc);
/*  */
void entity_template_copy_destroy(entity_core_copy *template, allocator alloc);

#endif
