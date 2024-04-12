
#include "target.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static void init(tarasque_entity *self_data)
{
    target *self = (target *) self_data;

    self->shape.body.local.scale = (vector2_t) { 1, 1 };
    self->shape.kind = SHAPE_2D_CIRCLE;
    self->shape.as_circle = (shape_2D_circle) { .radius = 20 };

    tarasque_entity_add_child(self, "visual", (tarasque_specific_entity) {
            .entity_def = BE_DEF_shape_2D_visual,
            .data = &(BE_shape_2D_visual) {
                    .color = (SDL_Color) { 0, 255, 0, 255 }, .draw_index = 5
            } });

    tarasque_entity_add_child(self, "collision", (tarasque_specific_entity) {
            .entity_def = BE_DEF_shape_2D_collider,
            .data = &(BE_shape_2D_collider) { 0u } });
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

tarasque_specific_entity target_entity(target *args)
{
    return (tarasque_specific_entity) {
            .data = args,
            .entity_def = {
                    .subtype = &BE_DEF_shape_2D,

                    .data_size = sizeof(*args),
                    .on_init = &init,
            }
    };
}
