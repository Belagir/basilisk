
#include <grafts/sdl_window.h>
#include <base_entities/sdl_entities.h>


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

    BE_window_sdl *parent_window = (BE_window_sdl *) tarasque_entity_get_parent(self_data, init_data->window_entity_name);

    init_data->renderer = SDL_CreateRenderer(parent_window->window, -1, init_data->flags);

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
 */
const tarasque_entity_definition BE_render_manager_sdl_entity_def = {
        .data_size = sizeof(BE_render_manager_sdl),
        .on_init = &BE_render_manager_sdl_init,
        .on_frame = &BE_render_manager_sdl_on_frame,
        .on_deinit = &BE_render_manager_sdl_deinit,
};
