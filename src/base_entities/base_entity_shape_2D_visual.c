
#include <base_entities/sdl_entities.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 */
typedef struct BE_shape_2D_visual {
    BE_shape_2D *visualized;

    SDL_Color color;
    i32 draw_index;
} BE_shape_2D_visual;

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
    SDL_FPoint octants[8u] = { 0u };

    f32 t1 = radius / 16.f;
    f32 t2 = 0.f;
    f32 x  = radius;
    f32 y  = 0.f;

    while (x > y) {
        octants[0u] = (SDL_FPoint) { .x = center.x + x, center.y + y };
        octants[1u] = (SDL_FPoint) { .x = center.x + y, center.y + x };
        octants[2u] = (SDL_FPoint) { .x = center.x - x, center.y + y };
        octants[3u] = (SDL_FPoint) { .x = center.x - y, center.y + x };
        octants[4u] = (SDL_FPoint) { .x = center.x + x, center.y - y };
        octants[5u] = (SDL_FPoint) { .x = center.x + y, center.y - x };
        octants[6u] = (SDL_FPoint) { .x = center.x - x, center.y - y };
        octants[7u] = (SDL_FPoint) { .x = center.x - y, center.y - x };

        SDL_RenderDrawPointsF(renderer, (const SDL_FPoint *) octants, 8u);

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

    visual->visualized = tarasque_entity_get_parent(visual, NULL, &BE_DEF_shape_2D);

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
    SDL_FRect rectangle = { 0u };

    SDL_SetRenderDrawColor(event_draw->renderer, visual->color.r, visual->color.g, visual->color.b, visual->color.a);

    switch (BE_shape_2D_what(visual->visualized)) {
        case SHAPE_2D_CIRCLE:
            BE_shape_2D_visual_render_draw_circle(event_draw->renderer, BE_body_2D_global(BE_shape_2D_get_body(visual->visualized)).position, BE_shape_2D_as_circle(visual->visualized)->radius);
            break;
        case SHAPE_2D_RECT:
            rectangle = (SDL_FRect) {
                    .x = BE_body_2D_global(BE_shape_2D_get_body(visual->visualized)).position.x,
                    .y = BE_body_2D_global(BE_shape_2D_get_body(visual->visualized)).position.y,
                    .w = BE_shape_2D_as_rect(visual->visualized)->width,
                    .h = BE_shape_2D_as_rect(visual->visualized)->height, };
            SDL_RenderDrawRectF(event_draw->renderer, &rectangle);
            break;
    }
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param color
 * @param draw_index
 * @return
 */
tarasque_entity *BE_STATIC_shape_2D_visual(SDL_Color color, i32 draw_index)
{
    static BE_shape_2D_visual buffer = { 0u };

    buffer = (BE_shape_2D_visual) { 0u };
    buffer = (BE_shape_2D_visual) {
            .color = color,
            .draw_index = draw_index,
    };

    return &buffer;
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
