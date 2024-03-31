
#include "background.h"

#include <SDL2/SDL_image.h>
#include <ustd/res.h>

DECLARE_RES(background_sprite, "res_bg_png")

static void init(tarasque_entity *entity)
{
    backround *bg = (backround *) entity;
    BE_render_manager_sdl *render_manager = (BE_render_manager_sdl *) tarasque_entity_get_parent(entity, NULL, &BE_render_manager_sdl_entity_def);

    bg->sprite.texture = IMG_LoadTexture_RW(render_manager->renderer, SDL_RWFromMem((void *) res__background_sprite_start, (int) ((size_t) res__background_sprite_end - (size_t) res__background_sprite_start)), 1);
    bg->sprite.body.local.scale = (vector2_t) { .75, .75 };
    bg->sprite.body.local.angle = 0.f;
}

static void deinit(tarasque_entity *entity)
{
    backround *bg = (backround *) entity;

    SDL_DestroyTexture(bg->sprite.texture);
    bg->sprite.texture = NULL;
}

tarasque_specific_entity backround_entity(backround *bg)
{
    return (tarasque_specific_entity) {
            .data = bg,
            .entity_def = {
                    .subtype = &BE_texture_2D_entity_def,

                    .on_init = &init,
                    .on_deinit = &deinit,

                    .data_size = sizeof(*bg),
            },
    };
}
