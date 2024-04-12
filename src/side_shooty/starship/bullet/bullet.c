
#include <grafts/sdl_window.h>
#include "bullet.h"

static void init(tarasque_entity *entity)
{
    bullet *self = (bullet *) entity;

    tarasque_entity_add_child(entity, "circle", (tarasque_specific_entity) {
            .entity_def = BE_DEF_shape_2D,
            .data = &(BE_shape_2D) {
                    .body = { .local.scale = { 1, 1 }, .local.position = { .x = 20, .y = 16 } }, .kind = SHAPE_2D_CIRCLE, .as_circle = { .radius = 10.f },
            } });

    tarasque_entity_add_child(tarasque_entity_get_child(entity, "circle", &BE_DEF_shape_2D), "circle visual", (tarasque_specific_entity) {
            .entity_def = BE_DEF_shape_2D_visual,
            .data = &(BE_shape_2D_visual) {
                    .color = (SDL_Color) { 255, 0, 0, 255 }, .draw_index = 5
            } });

    tarasque_entity_add_child(tarasque_entity_get_child(entity, "circle", &BE_DEF_shape_2D), "circle collision", (tarasque_specific_entity) {
            .entity_def = BE_DEF_shape_2D_collider,
            .data = &(BE_shape_2D_collider) { 0u } });
}

/**
 * @brief
 *
 * @param entity
 * @param elapsed_ms
 */
static void frame(tarasque_entity *entity, float elapsed_ms)
{
    bullet *self = (bullet *) entity;

    self->sprite.body.local.position.x += 20;

    if (self->sprite.body.local.position.x > 2000) {
        tarasque_entity_queue_remove(entity);
    }
}

/**
 * @brief
 *
 * @param args
 * @return
 */
tarasque_specific_entity bullet_entity(bullet *args)
{
    return (tarasque_specific_entity) {
        .data = args,
        .entity_def = {
                .subtype = &BE_DEF_texture_2D,

                .data_size = sizeof(*args),
                .on_init = &init,
                .on_frame = &frame,
        },
    };
}
