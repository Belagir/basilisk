
#include <grafts/sdl_window.h>
#include "bullet.h"

/**
 * @brief
 *
 * @param entity
 * @param event_data
 */
static void on_event_draw(tarasque_entity *entity, void *event_data)
{
    bullet *self = (bullet *) entity;
    BE_render_manager_sdl_event_draw *event_draw = (BE_render_manager_sdl_event_draw *) event_data;

    SDL_RenderCopy(event_draw->renderer, self->sprite, NULL, &(const SDL_Rect) { self->x, self->y, 32, 32 });
}

/**
 * @brief
 *
 * @param entity
 */
static void init(tarasque_entity *entity)
{
    tarasque_entity_queue_subscribe_to_event(entity, "sdl renderer draw", (tarasque_specific_event_subscription) { .callback = &on_event_draw, .index = 0, });
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

    self->x += 20;

    if (self->x > 2000) {
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
                .data_size = sizeof(*args),
                .on_init = &init,
                .on_frame = &frame,
        },
    };
}
