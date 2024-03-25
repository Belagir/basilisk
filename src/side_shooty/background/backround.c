
#include "background.h"

#include <SDL2/SDL_image.h>
#include <ustd/res.h>

#include <grafts/sdl_window.h>

DECLARE_RES(background_sprite, "res_bg_png")

static void on_draw(tarasque_entity *entity, void *event_data)
{
    backround *bg = (backround *) entity;
    be_render_manager_sdl_event_draw *draw_event = (be_render_manager_sdl_event_draw *) event_data;

    SDL_RenderCopy(draw_event->renderer, bg->texture, NULL, NULL);
}

static void init(tarasque_entity *entity)
{
    backround *bg = (backround *) entity;
    be_render_manager_sdl *render_manager = (be_render_manager_sdl *) tarasque_entity_get_parent(entity, "Render Manager");

    bg->texture = IMG_LoadTexture_RW(render_manager->renderer, SDL_RWFromMem((void *) res__background_sprite_start, (int) ((size_t) res__background_sprite_end - (size_t) res__background_sprite_start)), 1);

    if (bg->texture) {
        tarasque_entity_queue_subscribe_to_event(entity, "sdl renderer draw", (tarasque_specific_event_subscription) { .callback = on_draw, .index = -1 });
    }
}

static void deinit(tarasque_entity *entity)
{
    backround *bg = (backround *) entity;

    SDL_DestroyTexture(bg->texture);
    bg->texture = NULL;
}

tarasque_specific_entity backround_entity(backround *bg)
{
    return (tarasque_specific_entity) {
            .data_size = sizeof(&bg),
            .data = bg,
            .callbacks = {
                    .on_init = &init,
                    .on_deinit = &deinit,
            },
    };
}
