

#include <SDL2/SDL_image.h>

#include <ustd/res.h>
#include <base_entities/sdl_render_manager.h>

#include "starship.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

extern const unsigned char _binary_res_ship_png_start[];
extern const unsigned char _binary_res_ship_png_end[];

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param self_data
 * @param event_data
 */
static void on_draw(tarasque_entity *entity, void *event_data)
{
    starship *ship = (starship *) entity;
    base_entity_sdl_render_manager_event_draw *event_draw = (base_entity_sdl_render_manager_event_draw *) event_data;

    SDL_RenderCopy(event_draw->renderer, ship->sprite, NULL, &(const SDL_Rect) { ship->x, ship->y, 64, 64 });
}

/**
 * @brief
 *
 * @param entity
 */
static void init(tarasque_entity *entity)
{
    starship *ship = (starship *) entity;

    base_entity_sdl_render_manager_data *render_manager = (base_entity_sdl_render_manager_data *) tarasque_entity_get_parent(entity, "SDL Render Manager");

    if (render_manager) {
        ship->sprite = IMG_LoadTexture_RW(render_manager->renderer, SDL_RWFromConstMem(_binary_res_ship_png_start, (int) ((size_t) _binary_res_ship_png_end - (size_t) _binary_res_ship_png_start)), 0);
        tarasque_entity_subscribe_to_event(entity, "sdl renderer draw", (tarasque_specific_event_subscription) { .callback = &on_draw });
    }
}

/**
 * @brief
 *
 * @param entity
 */
static void deinit(tarasque_entity *entity)
{
    starship *ship = (starship *) entity;

    SDL_DestroyTexture(ship->sprite);
}

/**
 * @brief
 *
 * @param args
 * @return
 */
tarasque_specific_entity starship_entity(starship *args)
{
    return (tarasque_specific_entity) {
            .data_size = sizeof(*args),
            .data = args,
            .callbacks = {
                .on_init = &init,
                .on_deinit = &deinit,
            },
    };
}
