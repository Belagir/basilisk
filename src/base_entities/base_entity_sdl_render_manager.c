/**
 * @file base_entity_sdl_render_manager.c
 * @author gabriel ()
 * @brief Implementation file for the BE_render_manager_sdl base entity.
 * @version 0.1
 * @date 2024-04-27
 *
 * @copyright Copyright (c) 2024
 *
 */
#include <grafts/sdl_window.h>
#include <base_entities/sdl_entities.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Private data layout of a "render manager" base entity.
 * *
 * @see BE_STATIC_render_manager_sdl, BE_DEF_render_manager_sdl, BE_window_sdl
 */
typedef struct BE_render_manager_sdl {
    /** Color used to clear the screen each frame before each drawing operation. */
    SDL_Color clear_color;
    /** Flags passed to the SDL renderer creation method on initialisation of the entity. */
    SDL_RendererFlags flags;

    /** Width, in pixels, of the rendering buffer. */
    size_t w;
    /** Height, in pixels, of the rendering buffer. */
    size_t h;

    /** Pointer to a renderer that will be created on entity initialisation. Overriden on initialisation. */
    SDL_Renderer *renderer;
    /** Pointer to a texture used as a rendering buffer. */
    SDL_Texture *buffer;
} BE_render_manager_sdl;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* Initailize the internals needed by a BE_render_manager_sdl entity. */
static void BE_render_manager_sdl_init(tarasque_entity *self_data);

/* Releases the ressources taken by a BE_render_manager_sdl. */
static void BE_render_manager_sdl_deinit(tarasque_entity *self_data);

/* Frame callback to make a BE_render_manager_sdl send drawing events. */
static void BE_render_manager_sdl_on_frame(tarasque_entity *self_data, float elapsed_ms);

/* Event callback enacting operations needed to draw on the screen later. */
static void BE_render_manager_sdl_pre_draw(tarasque_entity *self_data, void *event_data);
/* Event callback enacting operations needed to present the pixel buffer to the screen. */
static void BE_render_manager_sdl_post_draw(tarasque_entity *self_data, void *event_data);
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Initialisation callback for a BE_render_manager_sdl entity.
 * Initializes the data of a render manager, creating a renderer and buffer to organize draw operations.
 * The renderer is bound to a parent BE_window_sdl entity. If no such entity is found, the renderer will not be created.
 *
 * @see BE_render_manager_sdl, BE_DEF_render_manager_sdl, BE_STATIC_render_manager_sdl
 *
 * @param[inout] self_data pointer to a BE_render_manager_sdl object
 */
static void BE_render_manager_sdl_init(tarasque_entity *self_data)
{
    if (!self_data) {
        return;
    }

    BE_render_manager_sdl *init_data = (BE_render_manager_sdl *) self_data;

    SDL_Window *parent_window = BE_window_sdl_get_window(tarasque_entity_get_parent(self_data, NULL, &BE_DEF_window_sdl));

    init_data->renderer = SDL_CreateRenderer(parent_window, -1, init_data->flags);

    if (init_data->renderer) {
        init_data->buffer = SDL_CreateTexture(init_data->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, (int) init_data->w, (int) init_data->h);

        tarasque_entity_queue_subscribe_to_event(self_data, "sdl renderer pre draw",  (tarasque_specific_event_subscription) { .callback = &BE_render_manager_sdl_pre_draw });
        tarasque_entity_queue_subscribe_to_event(self_data, "sdl renderer post draw", (tarasque_specific_event_subscription) { .callback = &BE_render_manager_sdl_post_draw });
    }
}

/**
 * @brief Deinitialisation callback for a BE_render_manager_sdl entity.
 * Releases the resources created by a render manager entity. This will destroy the renderer and the internal buffer.
 *
 * @see BE_render_manager_sdl, BE_DEF_render_manager_sdl, BE_STATIC_render_manager_sdl
 *
 * @param[inout] self_data pointer to a BE_render_manager_sdl object
 */
static void BE_render_manager_sdl_deinit(tarasque_entity *self_data)
{
    if (!self_data) {
        return;
    }

    BE_render_manager_sdl *data = (BE_render_manager_sdl *) self_data;

    SDL_DestroyTexture(data->buffer);
    SDL_DestroyRenderer(data->renderer);
    data->renderer = NULL;
}

/**
 * @brief Frame callback for a BE_render_manager_sdl entity.
 * Queue events to order drawing operations.
 *
 * @see BE_render_manager_sdl, BE_render_manager_sdl_event_draw
 *
 * @param[inout] self_data pointer to a BE_render_manager_sdl object
 * @param[in] elapsed_ms number of milliseconds elapsed since last frame
 */
static void BE_render_manager_sdl_on_frame(tarasque_entity *self_data, float elapsed_ms)
{
    if (!self_data) {
        return;
    }

    BE_render_manager_sdl *data = (BE_render_manager_sdl *) self_data;

    tarasque_entity_stack_event(self_data, "sdl renderer post draw", (tarasque_specific_event) { 0u });
    tarasque_entity_stack_event(self_data, "sdl renderer draw", (tarasque_specific_event) { .data_size = sizeof(BE_render_manager_sdl_event_draw), .data = &(BE_render_manager_sdl_event_draw) { data->renderer } });
    tarasque_entity_stack_event(self_data, "sdl renderer pre draw", (tarasque_specific_event) { 0u });
}

/**
 * @brief Pre-draw callback for a BE_render_manager_sdl entity.
 * This function will prepare the entity and SDL library for forthcoming SDL draw function calls.
 *
 * @see BE_render_manager_sdl, BE_render_manager_sdl_post_draw
 *
 * @param[inout] self_data pointer to a BE_render_manager_sdl object
 * @param[in] event_data discarded event data
 */
static void BE_render_manager_sdl_pre_draw(tarasque_entity *self_data, void *event_data)
{
    (void) event_data;

    if (!self_data) {
        return;
    }

    BE_render_manager_sdl *data = (BE_render_manager_sdl *) self_data;

    SDL_SetRenderTarget(data->renderer, data->buffer);
    SDL_SetRenderDrawColor(data->renderer, data->clear_color.r, data->clear_color.g, data->clear_color.b, data->clear_color.a);
    SDL_RenderClear(data->renderer);
}

/**
 * @brief Post-draw callback for a BE_render_manager_sdl entity.
 * This function will finalize drawing to the screen by copying its buffer to the main SDL render target and presenting it.
 *
 * @see BE_render_manager_sdl, BE_render_manager_sdl_pre_draw
 *
 * @param[inout] self_data pointer to a BE_render_manager_sdl object
 * @param[in] event_data discarded event data
 */
static void BE_render_manager_sdl_post_draw(tarasque_entity *self_data, void *event_data)
{
    (void) event_data;

    if (!self_data) {
        return;
    }

    BE_render_manager_sdl *data = (BE_render_manager_sdl *) self_data;

    SDL_RenderPresent(data->renderer);

    SDL_SetRenderTarget(data->renderer, NULL);
    SDL_RenderCopy(data->renderer, data->buffer, NULL, NULL);
    SDL_RenderPresent(data->renderer);
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Returns a statically allocated BE_render_manager_sdl object constructed from the given configuration.
 * Successive calls to this function will always yield the same object, with some eventual differing content (depending on the given arguments).
 * Use this to build new BE_render_manager_sdl entity instances with a call to `tarasque_entity_add_child()` that will copy the data inside the returned object.
 *
 * @see BE_render_manager_sdl, BE_DEF_render_manager_sdl, BE_window_sdl
 *
 * @param[in] clear_color default color of the renderer background
 * @param[in] w width, in pixels, of the renderer's buffer ; a value of 0 will make it match the size of the parent window
 * @param[in] h height, in pixels,  of the renderer's buffer ; a value of 0 will make it match the size of the parent window
 * @param[in] flags SDL-specific renderer flags
 * @return tarasque_entity *
 */
tarasque_entity *BE_STATIC_render_manager_sdl(SDL_Color clear_color, size_t w, size_t h, SDL_RendererFlags flags)
{
    static BE_render_manager_sdl buffer = { 0u };

    buffer = (BE_render_manager_sdl) { 0u };
    buffer = (BE_render_manager_sdl) {
            .clear_color = clear_color,
            .w = w, .h = h,
            .flags = flags,
    };

    return &buffer;
}

// -------------------------------------------------------------------------------------------------

/**
 * @brief Defines the properties of a BE_render_manager_sdl entity.
 *
 * This entity exists to organize drawing operation in a single time unit. To achieve that, the entity will send three events on each frame : "sdl renderer pre draw", "sdl renderer draw" and "sdl renderer post draw". Those three events are guaranteed to be resolved in this specific order, and entities that want to call SDL drawing functions can do so when receiving a "sdl renderer draw" event, which is associated to the `BE_render_manager_sdl_event_draw` data structure.
 *
 * This entity might send three events :
 * - "sdl renderer pre draw", not associated to any data, to notify that drawing operations will happen next ;
 * - "sdl renderer draw", associated to a BE_render_manager_sdl_event_draw object, to quiery draw operations to be done ;
 * - "sdl renderer post draw", not associated to any data, to notify that drawing operations ended.
 *
 * @see BE_render_manager_sdl, BE_STATIC_render_manager_sdl, BE_window_sdl, BE_render_manager_sdl_event_draw
 *
 */
const tarasque_entity_definition BE_DEF_render_manager_sdl = {
        .data_size = sizeof(BE_render_manager_sdl),
        .on_init = &BE_render_manager_sdl_init,
        .on_frame = &BE_render_manager_sdl_on_frame,
        .on_deinit = &BE_render_manager_sdl_deinit,
};
