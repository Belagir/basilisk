
#include <base_entities/sdl_entities.h>

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

    BE_texture_2D *texture_data = (BE_texture_2D *) self_data;

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
    if (!self_data || !event_data) {
        return;
    }

    int text_w = 0;
    int text_h = 0;

    BE_texture_2D *texture_data = (BE_texture_2D *) self_data;
    BE_render_manager_sdl_event_draw *draw_data = (BE_render_manager_sdl_event_draw *) event_data;

    SDL_QueryTexture(texture_data->texture, NULL, NULL, &text_w, &text_h);

    // SDL_RenderCopyEx(
    //         draw_data->renderer,
    //         texture_data->texture,
    //         NULL,
    //         &(const SDL_Rect) {
    //                 .x = (int) texture_data->body.global.position.x,
    //                 .y = (int) texture_data->body.global.position.y,
    //                 .w = (int) (texture_data->body.global.scale.x * (f32) text_w),
    //                 .h = (int) (texture_data->body.global.scale.y * (f32) text_h),
    //         },
    //         texture_data->body.global.angle,
    //         NULL,
    //         SDL_FLIP_NONE
    // );
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 */
const tarasque_entity_definition BE_DEF_texture_2D = {
        // .subtype = &BE_DEF_body_2D,

        .data_size = sizeof(BE_texture_2D),

        .on_init = &BE_texture_2D_init,
};
