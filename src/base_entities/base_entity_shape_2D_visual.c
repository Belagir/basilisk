/**
 * @file base_entity_shape_2D_visual.c
 * @author gabriel ()
 * @brief Implementation file for the shape visual basic entity.
 * @version 0.1
 * @date 2024-05-03
 *
 * @copyright Copyright (c) 2024
 *
 */
#include <base_entities/sdl_entities.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Private layout of a shape visual entity.
 *
 * @see BE_DEF_shape_2D_visual, BE_STATIC_shape_2D_visual, BE_shape_2D
 *
 */
typedef struct BE_shape_2D_visual {
    /** Pointer to a parent shape entity. */
    BE_shape_2D *visualized;

    /** Color of the shape. */
    SDL_Color color;
    /** Draw index of the rendered shape. */
    i32 draw_index;
} BE_shape_2D_visual;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* Utility function to draw an unfilled circle to an rendering context. */
static void BE_shape_2D_visual_render_draw_circle(SDL_Renderer *renderer, vector2_t center, f32 radius);

/* Initialisation callback for a BE_shape_2D_visual entity. */
static void BE_shape_2D_visual_on_init(tarasque_entity *self_data);

/* Draw event callback for a BE_shape_2D_visual entity. */
static void BE_shape_2D_visual_on_draw(tarasque_entity *self_data, void *event_data);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Draws a colored, unfilled circle to a SDL context.
 *
 * TODO : fix octants sometimes not properly connecting & make it possible to fill the circle
 *
 * @param[in] renderer pointer to a SDL rendering context
 * @param[in] x horizontal axis pixel coordinates of the circle's center
 * @param[in] y vertical axis pixel coordinates of the circle's center
 * @param[in] radius radius, in pixels, of the circle
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
 * @brief Initialises a BE_shape_2D_visual entity.
 * The function will try to find a parent `BE_DEF_shape_2D` to hook to and pull shape / position information from.
 *
 * @param[inout] self_data pointer to a BE_DEF_shape_2D object
 */
static void BE_shape_2D_visual_on_init(tarasque_entity *self_data)
{
    BE_shape_2D_visual *visual = (BE_shape_2D_visual *) self_data;

    visual->visualized = tarasque_entity_get_parent(visual, NULL, &BE_DEF_shape_2D);

    if (visual->visualized) {
        tarasque_entity_queue_subscribe_to_event(visual, "sdl renderer draw",
                (tarasque_specific_event_subscription) { .callback = &BE_shape_2D_visual_on_draw, .index = visual->draw_index, });
    }
}

/**
 * @brief Draws a BE_shape_2D_visual entity using the informations contained in a `BE_render_manager_sdl_event_draw` object.
 * The function will draw the parent shape entity it might have detected with `BE_shape_2D_visual_on_init()`.
 *
 * @param[inout] self_data pointer to a BE_DEF_shape_2D object
 * @param[inout] event_data pointer to a BE_render_manager_sdl_event_draw` object
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
 * @brief Returns a statically allocated BE_STATIC_shape_2D_visual constructed as an axis-aligned rectangle.
 * Successive calls to this function will always yield the same object, with some eventual differing content (depending on the given arguments).
 * Use this to build new BE_shape_2D_visual instances with a call to `tarasque_entity_add_child()` that will copy the data inside the returned object.
 *
 * @see BE_shape_2D_visual, BE_DEF_shape_2D_visual, BE_shape_2D
 *
 * @param[in] color color of the drawn shape
 * @param[in] draw_index drazw index of the drawn shape
 * @return tarasque_entity *
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
 * @brief Defines the properties of a shape visual entity.
 *
 * A shape visual is meant to be a child of a BE_shape_2D entity, and if it is not, the entity will have no effect.
 * The BE_shape_2D_visual entity provides a way to extend the functionalities of a BE_shape_2D entity so it can be rendered to the screen.
 *
 * @see BE_shape_2D_visual, BE_STATIC_shape_2D_visual, BE_shape_2D
 *
 */
const tarasque_entity_definition BE_DEF_shape_2D_visual = {
        .data_size = sizeof(BE_shape_2D_visual),

        .on_init = &BE_shape_2D_visual_on_init,
};
