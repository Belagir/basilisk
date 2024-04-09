

#include <SDL2/SDL_image.h>

#include <ustd/res.h>
#include <grafts/sdl_window.h>

#include "starship.h"

#include "bullet/bullet.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

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

    BE_render_manager_sdl *render_manager = (BE_render_manager_sdl *) tarasque_entity_get_parent(entity, NULL, &BE_DEF_render_manager_sdl);
    void *sprite_png_data = NULL;
    void *bullet_png_data = NULL;
    size_t sprite_png_size = 0u;
    size_t bullet_png_size = 0u;

    if (!render_manager) {
        return;
    }

    ship->sprite.body.local.scale = (vector2_t) { 1, 1 };

    sprite_png_data = tarasque_entity_fetch_resource(entity, "sprites", "res/ship.png", &sprite_png_size);
    bullet_png_data = tarasque_entity_fetch_resource(entity, "sprites", "res/bullet.png", &bullet_png_size);

    ship->sprite.texture = IMG_LoadTexture_RW(render_manager->renderer, SDL_RWFromConstMem(sprite_png_data, (int) sprite_png_size), 1);
    ship->bullets_sprite = IMG_LoadTexture_RW(render_manager->renderer, SDL_RWFromConstMem(bullet_png_data, (int) bullet_png_size), 1);

    tarasque_entity_queue_subscribe_to_event(entity, "sdl event", (tarasque_specific_event_subscription) { .callback = &on_sdl_event });
}

/**
 * @brief
 *
 * @param entity
 */
static void deinit(tarasque_entity *entity)
{
    starship *ship = (starship *) entity;

    SDL_DestroyTexture(ship->sprite.texture);
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

    ship->sprite.body.local.position.y += (float) ship->vel_y;

    if (ship->is_shooting) {

        tarasque_entity_add_child(tarasque_entity_get_parent(entity, NULL, NULL), "bullet", bullet_entity(&(bullet) {
                .sprite = {
                        .body = { .local = { .scale = { 1, 1 }, .position = ship->sprite.body.local.position } },
                        .draw_index = 1, .texture = ship->bullets_sprite,
                },
        }));
        tarasque_entity_add_child(tarasque_entity_get_parent(entity, NULL, NULL), "bullet", bullet_entity(&(bullet) {
                .sprite = {
                        .body = { .local = { .scale = { 1, 1 }, .position = vector2_add(ship->sprite.body.local.position, (vector2_t) { 0, 32 }) } },
                        .draw_index = 1, .texture = ship->bullets_sprite,
                },
        }));

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
            .data = args,
            .entity_def = {
                    .subtype = &BE_DEF_texture_2D,

                    .data_size = sizeof(*args),
                    .on_init = &init,
                    .on_deinit = &deinit,
                    .on_frame = &frame,
            },
    };
}
