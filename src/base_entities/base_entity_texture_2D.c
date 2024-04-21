
#include <base_entities/sdl_entities.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Data layout of a "2D texture" entity. Use it to configure what texture is displayed, on what index, and where.
 */
typedef struct BE_texture_2D_impl {
    /** Position information of the texture entity. */
    tarasque_entity *body;
    /** Draw index to register a drawing operation on initialization. */
    i32 draw_index;

    /** Texture rendered to the screen. */
    SDL_Texture *texture;
} BE_texture_2D_impl;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/*  */
static void BE_texture_2D_init(tarasque_entity *self_data);

/*  */
static void BE_texture_2D_on_draw(tarasque_entity *self_data, void *event_data);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param self_data
 */
static void BE_texture_2D_init(tarasque_entity *self_data)
{
    if (!self_data) {
        return;
    }

    BE_texture_2D_impl *texture_data = (BE_texture_2D_impl *) self_data;

    texture_data->body = tarasque_entity_get_parent(self_data, NULL, &BE_DEF_body_2D);

    tarasque_entity_queue_subscribe_to_event(self_data, "sdl renderer draw", (tarasque_specific_event_subscription) { .index = texture_data->draw_index, .callback = &BE_texture_2D_on_draw });
}

/**
 * @brief
 *
 * @param self_data
 * @param event_draw
 */
static void BE_texture_2D_on_draw(tarasque_entity *self_data, void *event_data)
{
    SDL_Rect dest = { 0u };

    if (!self_data || !event_data) {
        return;
    }

    int text_w = 0;
    int text_h = 0;

    BE_texture_2D_impl *texture_data = (BE_texture_2D_impl *) self_data;
    BE_render_manager_sdl_event_draw *draw_data = (BE_render_manager_sdl_event_draw *) event_data;

    SDL_QueryTexture(texture_data->texture, NULL, NULL, &text_w, &text_h);

    dest = (SDL_Rect) { .x = 0, .y = 0, .w = text_w, .h = text_h, };
    if (texture_data->body) {
        dest.x = (int) BE_body_2D_get(texture_data->body, BODY_2D_SPACE_GLOBAL).position.x;
        dest.y = (int) BE_body_2D_get(texture_data->body, BODY_2D_SPACE_GLOBAL).position.y;
        dest.w = (int) (BE_body_2D_get(texture_data->body, BODY_2D_SPACE_GLOBAL).scale.x * (f32) text_w);
        dest.h = (int) (BE_body_2D_get(texture_data->body, BODY_2D_SPACE_GLOBAL).scale.y * (f32) text_h);
    }

    SDL_RenderCopyEx(
            draw_data->renderer,
            texture_data->texture,
            NULL,
            &dest,
            0.f, // TODO : angle
            NULL,
            SDL_FLIP_NONE
    );
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 */
const tarasque_entity_definition BE_DEF_texture_2D = {
        .data_size = sizeof(BE_texture_2D_impl),

        .on_init = &BE_texture_2D_init,
};
