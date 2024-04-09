
#include "background.h"

#include <SDL2/SDL_image.h>
#include <ustd/res.h>

static void init(tarasque_entity *entity)
{
    backround *bg = (backround *) entity;
    BE_render_manager_sdl *render_manager = (BE_render_manager_sdl *) tarasque_entity_get_parent(entity, NULL, &BE_DEF_render_manager_sdl);

    void *sprite_png_data = NULL;
    size_t sprite_png_size = 0u;

    sprite_png_data = tarasque_entity_fetch_resource(entity, "background", "res/bg.png", &sprite_png_size);

    bg->sprite.texture = IMG_LoadTexture_RW(render_manager->renderer, SDL_RWFromMem(sprite_png_data, (int) sprite_png_size), 1);
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
                    .subtype = &BE_DEF_texture_2D,

                    .on_init = &init,
                    .on_deinit = &deinit,

                    .data_size = sizeof(*bg),
            },
    };
}
