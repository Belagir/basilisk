
#include <grafts/sdl2_window.h>

#include "graft_entities.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 */
typedef struct graft_entity_sdl_render_manager_data {
    graft_entity_sdl_render_manager_args init;

    SDL_Renderer *renderer;
} graft_entity_sdl_render_manager_data;

/**
 * @brief
 *
 */
static graft_entity_sdl_render_manager_data graft_entity_sdl_render_manager_data_buffer = { 0u };

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/*  */
static void graft_entity_sdl_render_manager_init(tarasque_entity *self_data);

/*  */
static void graft_entity_sdl_render_manager_deinit(tarasque_entity *self_data);

/*  */
static void graft_entity_sdl_render_manager_on_frame(tarasque_entity *self_data, float elapsed_ms);

/*  */
static void graft_entity_sdl_render_manager_pre_draw(tarasque_entity *self_data, void *event_data);
/*  */
static void graft_entity_sdl_render_manager_post_draw(tarasque_entity *self_data, void *event_data);
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param self_data
 */
static void graft_entity_sdl_render_manager_init(tarasque_entity *self_data)
{
    if (!self_data) {
        return;
    }

    graft_entity_sdl_render_manager_data *init_data = (graft_entity_sdl_render_manager_data *) self_data;
    init_data->renderer = SDL_CreateRenderer(init_data->init.source_window, -1, init_data->init.args.flags);

    if (init_data->renderer) {
        tarasque_entity_subscribe_to_event(self_data, "sdl renderer pre draw",  (tarasque_specific_event_subscription) { &graft_entity_sdl_render_manager_pre_draw });
        tarasque_entity_subscribe_to_event(self_data, "sdl renderer post draw", (tarasque_specific_event_subscription) { &graft_entity_sdl_render_manager_post_draw });
    }
}

/**
 * @brief
 *
 * @param self_data
 */
static void graft_entity_sdl_render_manager_deinit(tarasque_entity *self_data)
{
    if (!self_data) {
        return;
    }

    graft_entity_sdl_render_manager_data *data = (graft_entity_sdl_render_manager_data *) self_data;

    SDL_DestroyRenderer(data->renderer);
    data->renderer = NULL;
}

/**
 * @brief
 *
 * @param self_data
 * @param elapsed_ms
 */
static void graft_entity_sdl_render_manager_on_frame(tarasque_entity *self_data, float elapsed_ms)
{
    if (!self_data) {
        return;
    }

    graft_entity_sdl_render_manager_data *data = (graft_entity_sdl_render_manager_data *) self_data;

    tarasque_entity_stack_event(self_data, "sdl renderer post draw", (tarasque_specific_event) { 0u });
    tarasque_entity_stack_event(self_data, "sdl renderer draw", (tarasque_specific_event) { .data_size = sizeof(graft_sdl_window_event_draw), .data = &(graft_sdl_window_event_draw) { data->renderer } });
    tarasque_entity_stack_event(self_data, "sdl renderer pre draw", (tarasque_specific_event) { 0u });
}

/**
 * @brief
 *
 * @param self_data
 * @param event_data
 */
static void graft_entity_sdl_render_manager_pre_draw(tarasque_entity *self_data, void *event_data)
{
    (void) event_data;

    if (!self_data) {
        return;
    }

    graft_entity_sdl_render_manager_data *data = (graft_entity_sdl_render_manager_data *) self_data;

    SDL_SetRenderTarget(data->renderer, NULL);
    SDL_SetRenderDrawColor(data->renderer,
            data->init.args.clear_color.r,
            data->init.args.clear_color.g,
            data->init.args.clear_color.b,
            data->init.args.clear_color.a);
    SDL_RenderClear(data->renderer);
}

/**
 * @brief
 *
 * @param self_data
 * @param event_data
 */
static void graft_entity_sdl_render_manager_post_draw(tarasque_entity *self_data, void *event_data)
{
    (void) event_data;

    if (!self_data) {
        return;
    }

    graft_entity_sdl_render_manager_data *data = (graft_entity_sdl_render_manager_data *) self_data;

    SDL_RenderPresent(data->renderer);
}
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param args
 * @return
 */
tarasque_specific_entity graft_entity_sdl_render_manager(graft_entity_sdl_render_manager_args args)
{
    // avoiding stack allocation but still won't force the user to deal with the heap...
    graft_entity_sdl_render_manager_data_buffer = (graft_entity_sdl_render_manager_data) {
            .init = args,
            .renderer = NULL,
    };

    return (tarasque_specific_entity) {
            .data_size = sizeof(graft_entity_sdl_render_manager_data_buffer),
            .data = &graft_entity_sdl_render_manager_data_buffer,

            .callbacks = {
                    .on_init = &graft_entity_sdl_render_manager_init,
                    .on_frame = &graft_entity_sdl_render_manager_on_frame,
                    .on_deinit = &graft_entity_sdl_render_manager_deinit,
            }
    };
}
