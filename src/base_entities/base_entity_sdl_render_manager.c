
#include <grafts/sdl_window.h>
#include <base_entities/sdl_entities.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Data layout of an "render manager" base entity. Use it to configure the clear color and flags on entity creation.
 */
typedef struct BE_render_manager_sdl {
    /** Color used to clear the screen each frame before each drawing operation. */
    SDL_Color clear_color;
    /** Flags passed to the SDL renderer creation method on initialization of the entity. */
    SDL_RendererFlags flags;

    /** Pointer to a renderer that will be created on entity initialization. Overriden on initialization. */
    SDL_Renderer *renderer;
} BE_render_manager_sdl;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/*  */
static void BE_render_manager_sdl_init(tarasque_entity *self_data);

/*  */
static void BE_render_manager_sdl_deinit(tarasque_entity *self_data);

/*  */
static void BE_render_manager_sdl_on_frame(tarasque_entity *self_data, float elapsed_ms);

/*  */
static void BE_render_manager_sdl_pre_draw(tarasque_entity *self_data, void *event_data);
/*  */
static void BE_render_manager_sdl_post_draw(tarasque_entity *self_data, void *event_data);
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param self_data
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
        tarasque_entity_queue_subscribe_to_event(self_data, "sdl renderer pre draw",  (tarasque_specific_event_subscription) { .callback = &BE_render_manager_sdl_pre_draw });
        tarasque_entity_queue_subscribe_to_event(self_data, "sdl renderer post draw", (tarasque_specific_event_subscription) { .callback = &BE_render_manager_sdl_post_draw });
    }
}

/**
 * @brief
 *
 * @param self_data
 */
static void BE_render_manager_sdl_deinit(tarasque_entity *self_data)
{
    if (!self_data) {
        return;
    }

    BE_render_manager_sdl *data = (BE_render_manager_sdl *) self_data;

    SDL_DestroyRenderer(data->renderer);
    data->renderer = NULL;
}

/**
 * @brief
 *
 * @param self_data
 * @param elapsed_ms
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
 * @brief
 *
 * @param self_data
 * @param event_data
 */
static void BE_render_manager_sdl_pre_draw(tarasque_entity *self_data, void *event_data)
{
    (void) event_data;

    if (!self_data) {
        return;
    }

    BE_render_manager_sdl *data = (BE_render_manager_sdl *) self_data;

    SDL_SetRenderTarget(data->renderer, NULL);
    SDL_SetRenderDrawColor(data->renderer, data->clear_color.r, data->clear_color.g, data->clear_color.b, data->clear_color.a);
    SDL_RenderClear(data->renderer);
}

/**
 * @brief
 *
 * @param self_data
 * @param event_data
 */
static void BE_render_manager_sdl_post_draw(tarasque_entity *self_data, void *event_data)
{
    (void) event_data;

    if (!self_data) {
        return;
    }

    BE_render_manager_sdl *data = (BE_render_manager_sdl *) self_data;

    SDL_RenderPresent(data->renderer);
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param clear_color
 * @param flags
 * @return
 */
tarasque_entity *BE_STATIC_render_manager_sdl(SDL_Color clear_color, SDL_RendererFlags flags)
{
    static BE_render_manager_sdl buffer = { 0u };

    buffer = (BE_render_manager_sdl) { 0u };
    buffer = (BE_render_manager_sdl) {
            .clear_color = clear_color,
            .flags = flags,
    };

    return &buffer;
}

// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 */
const tarasque_entity_definition BE_DEF_render_manager_sdl = {
        .data_size = sizeof(BE_render_manager_sdl),
        .on_init = &BE_render_manager_sdl_init,
        .on_frame = &BE_render_manager_sdl_on_frame,
        .on_deinit = &BE_render_manager_sdl_deinit,
};
