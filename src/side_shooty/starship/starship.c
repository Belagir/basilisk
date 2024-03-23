

#include <SDL2/SDL_image.h>

#include <ustd/res.h>
#include <grafts/sdl_window.h>

#include "starship.h"

#include "bullet/bullet.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

DECLARE_RES(ship_sprite, "res_ship_png")
DECLARE_RES(bullet_sprite, "res_bullet_png")

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
    be_render_manager_sdl_event_draw *event_draw = (be_render_manager_sdl_event_draw *) event_data;

    SDL_RenderCopy(event_draw->renderer, ship->sprite, NULL, &(const SDL_Rect) { ship->x, ship->y, 64, 64 });
}

/**
 * @brief
 *
 * @param entity
 * @param event_data
 */
static void on_sdl_event(tarasque_entity *entity, void *event_data)
{
    SDL_Event *event = (SDL_Event *) event_data;
    starship *ship = (starship *) entity;

    if ((event->type == SDL_KEYDOWN) && (event->key.keysym.scancode == SDL_SCANCODE_UP)) {
        ship->vel_y = -5;
    }
    if ((event->type == SDL_KEYDOWN) && (event->key.keysym.scancode == SDL_SCANCODE_DOWN)) {
        ship->vel_y =  5;
    }

    if ((event->type == SDL_KEYUP) && (event->key.keysym.scancode == SDL_SCANCODE_UP)) {
        ship->vel_y = 0;
    }

    if ((event->type == SDL_KEYUP) && (event->key.keysym.scancode == SDL_SCANCODE_DOWN)) {
        ship->vel_y = 0;
    }

    if ((event->type == SDL_KEYUP) && (event->key.keysym.scancode == SDL_SCANCODE_SPACE)) {
        ship->is_shooting = true;
    }
}

/**
 * @brief
 *
 * @param entity
 */
static void init(tarasque_entity *entity)
{
    starship *ship = (starship *) entity;

    be_render_manager_sdl *render_manager = (be_render_manager_sdl *) tarasque_entity_get_parent(entity, "Render Manager");

    if (!render_manager) {
        return;
    }

    ship->sprite = IMG_LoadTexture_RW(render_manager->renderer, SDL_RWFromConstMem(res__ship_sprite_start, (int) ((size_t) res__ship_sprite_end - (size_t) res__ship_sprite_start)), 1);
    ship->bullets_sprite = IMG_LoadTexture_RW(render_manager->renderer, SDL_RWFromConstMem(res__bullet_sprite_start, (int) ((size_t) res__bullet_sprite_end - (size_t) res__bullet_sprite_start)), 1);

    tarasque_entity_subscribe_to_event(entity, "sdl renderer draw", (tarasque_specific_event_subscription) { .callback = &on_draw, .priority = 1 });
    tarasque_entity_subscribe_to_event(entity, "sdl event", (tarasque_specific_event_subscription) { .callback = &on_sdl_event });
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
    SDL_DestroyTexture(ship->bullets_sprite);
}

/**
 * @brief
 *
 * @param entity
 * @param elapsed_ms
 */
static void frame(tarasque_entity *entity, float elapsed_ms)
{
    starship *ship = (starship *) entity;

    ship->y += ship->vel_y;

    if (ship->is_shooting) {
        tarasque_entity_add_child(entity, "", "bullet", bullet_entity(&(bullet) { .x = ship->x, .y = ship->y, .sprite = ship->bullets_sprite }));
        tarasque_entity_add_child(entity, "", "bullet", bullet_entity(&(bullet) { .x = ship->x, .y = ship->y + 32, .sprite = ship->bullets_sprite }));
        ship->is_shooting = false;
    }
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
                .on_frame = &frame,
            },
    };
}
