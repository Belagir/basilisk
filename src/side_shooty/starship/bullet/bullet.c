
#include <grafts/sdl_window.h>
#include "bullet.h"

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
                .subtype = &BE_texture_2D_entity_def,

                .data_size = sizeof(*args),
                .on_frame = &frame,
        },
    };
}
