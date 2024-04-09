
#include <base_entities/sdl_entities.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/*  */
static void BE_shape_2D_visual_render_draw_circle(SDL_Renderer *renderer, vector2_t center, f32 radius);

/*  */
static void BE_shape_2D_visual_on_init(tarasque_entity *self_data);

/*  */
static void BE_shape_2D_visual_on_draw(tarasque_entity *self_data, void *event_data);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param
 * @param x
 * @param y
 * @param radius
 */
static void BE_shape_2D_visual_render_draw_circle(SDL_Renderer *renderer, vector2_t center, f32 radius)
{
    f32 t1 = radius / 16.f;
    f32 t2 = 0.f;
    f32 x = radius;
    f32 y = 0.f;

    while (x > y) {
        SDL_RenderDrawPointF(renderer, x, y);

        y = y + 1.f;
        t1 = t1 + y;
        t2 = t1 - x;

        if (t2 >= 0.f) {
            t1 = t2;
            x = x - 1.f;
        }
    }
}

/**
 * @brief
 *
 * @param self_data
 */
static void BE_shape_2D_visual_on_init(tarasque_entity *self_data)
{
    BE_shape_2D_visual *visual = (BE_shape_2D_visual *) self_data;

    visual->visualized = (BE_shape_2D *) tarasque_entity_get_parent(visual, NULL, &BE_DEF_shape_2D);

    tarasque_entity_queue_subscribe_to_event(visual, "sdl renderer draw",
            (tarasque_specific_event_subscription) { .callback = &BE_shape_2D_visual_on_draw, .index = visual->draw_index, });
}

/**
 * @brief
 *
 * @param self_data
 * @param event_data
 */
static void BE_shape_2D_visual_on_draw(tarasque_entity *self_data, void *event_data)
{
    BE_shape_2D_visual *visual = (BE_shape_2D_visual *) self_data;
    BE_render_manager_sdl_event_draw *event_draw = (BE_render_manager_sdl_event_draw *) event_data;

    switch (visual->visualized->kind) {
        case SHAPE_2D_CIRCLE:
            BE_shape_2D_visual_render_draw_circle(event_draw->renderer,
                    visual->visualized->body.global.position, visual->visualized->as_circle.radius);
            break;
        case SHAPE_2D_RECT:
            SDL_RenderDrawRectF(event_draw->renderer, &(const SDL_FRect) {
                    .x = visual->visualized->body.global.position.x,
                    .y = visual->visualized->body.global.position.y,
                    .h = visual->visualized->as_rect.height,
                    .w = visual->visualized->as_rect.width, });
            break;
    }
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 */
const tarasque_entity_definition BE_DEF_shape_2D_visual = {
        .data_size = sizeof(BE_shape_2D_visual),

        .on_init = &BE_shape_2D_visual_on_init,
};
