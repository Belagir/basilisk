/**
 * @file base_entity_texture_2D.c
 * @author gabriel ()
 * @brief Implementation file for the BE_texture_2D entity.
 * @version 0.1
 * @date 2024-04-27
 *
 * @copyright Copyright (c) 2024
 *
 */
#include <base_entities/sdl_entities.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Private data layout of a "2D texture" entity.
 *
 * @see BE_STATIC_texture_2D, ENTITY_DEF_TEXTURE_2D
 */
typedef struct BE_texture_2D {
    /** Position information of the texture entity. */
    basilisk_entity *body;
    /** Draw index to register a drawing operation on initialisation. */
    i32 draw_index;

    /** Texture rendered to the screen. */
    SDL_Texture *texture;
} BE_texture_2D;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* Initialises the texture by finding its position by finding a BE_2D_body parent. */
static void BE_texture_2D_init(basilisk_entity *self_data);

/* Draw event callback to render the texture to the current rendering target. */
static void BE_texture_2D_on_draw(basilisk_entity *self_data, void *event_data);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Initialise the entity. This function will find the first BE_2D_body parent entity to pull its position from.
 *
 * @param[inout] self_data pointer to BE_texture_2D object
 */
static void BE_texture_2D_init(basilisk_entity *self_data)
{
    if (!self_data) {
        return;
    }

    BE_texture_2D *texture_data = (BE_texture_2D *) self_data;

    texture_data->body = basilisk_entity_get_parent(self_data, NULL, &ENTITY_DEF_BODY_2D);

    basilisk_entity_queue_subscribe_to_event(self_data, "sdl renderer draw", (basilisk_specific_event_subscription) { .index = texture_data->draw_index, .callback = &BE_texture_2D_on_draw });
}

/**
 * @brief Draws the entity to the current rendering target.
 * Takes some pointer to event data that must be a BE_render_manager_sdl_event_draw object.
 *
 * @param[inout] self_data pointer to BE_texture_2D object
 * @param[in] event_draw pointer to a BE_render_manager_sdl_event_draw object
 */
static void BE_texture_2D_on_draw(basilisk_entity *self_data, void *event_data)
{
    SDL_Rect dest = { 0u };

    if (!self_data || !event_data) {
        return;
    }

    int text_w = 0;
    int text_h = 0;

    BE_texture_2D *texture_data = (BE_texture_2D *) self_data;
    BE_render_manager_sdl_event_draw *draw_data = (BE_render_manager_sdl_event_draw *) event_data;

    SDL_QueryTexture(texture_data->texture, NULL, NULL, &text_w, &text_h);

    dest = (SDL_Rect) { .x = 0, .y = 0, .w = text_w, .h = text_h, };
    if (texture_data->body) {
        dest.x = (int) body_2D_global(texture_data->body).position.x;
        dest.y = (int) body_2D_global(texture_data->body).position.y;
        dest.w = (int) (body_2D_global(texture_data->body).scale.x * (f32) text_w);
        dest.h = (int) (body_2D_global(texture_data->body).scale.y * (f32) text_h);
    }

    SDL_RenderCopyEx(
            draw_data->renderer,
            texture_data->texture,
            NULL,
            &dest,
            body_2D_global(texture_data->body).angle,
            NULL,
            SDL_FLIP_NONE
    );
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Defines the entity properties of a BE_texture_2D entity.
 *
 * This entity provides a wrapper around a SDL texture to render it when a "sdl renderer draw" event is received. This entity will know where to draw the texture by finding a BE_2D_body parent and pulling the positionning information it needs from it. If such a parent is not found, the texture will be rendered as-is on the top left corner of the screen.
 *
 * @see BE_STATIC_texture_2D, BE_texture_2D
 */
const basilisk_entity_definition ENTITY_DEF_TEXTURE_2D = {
        .data_size = sizeof(BE_texture_2D),

        .on_init = &BE_texture_2D_init,
};

struct basilisk_specific_entity create_texture_2D(SDL_Texture *texture, i32 draw_index)
{
    static BE_texture_2D buffer = { 0u };

    if (!texture) {
        return (struct basilisk_specific_entity) { 0u };
    }

    buffer = (BE_texture_2D) { 0u };
    buffer = (BE_texture_2D) {
            .texture = texture,
            .draw_index = draw_index,
    };

    return (struct basilisk_specific_entity) {
            .entity_def = ENTITY_DEF_TEXTURE_2D,
            .data = &buffer,
    };
}