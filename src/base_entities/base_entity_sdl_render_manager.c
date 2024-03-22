
#include <base_entities/sdl_render_manager.h>
#include <base_entities/sdl_window.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/*  */
static void base_entity_sdl_render_manager_init(tarasque_entity *self_data);

/*  */
static void base_entity_sdl_render_manager_deinit(tarasque_entity *self_data);

/*  */
static void base_entity_sdl_render_manager_on_frame(tarasque_entity *self_data, float elapsed_ms);

/*  */
static void base_entity_sdl_render_manager_pre_draw(tarasque_entity *self_data, void *event_data);
/*  */
static void base_entity_sdl_render_manager_post_draw(tarasque_entity *self_data, void *event_data);
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param self_data
 */
static void base_entity_sdl_render_manager_init(tarasque_entity *self_data)
{
    if (!self_data) {
        return;
    }

    base_entity_sdl_render_manager_data *init_data = (base_entity_sdl_render_manager_data *) self_data;

    base_entity_sdl_window_data *parent_window = (base_entity_sdl_window_data *) tarasque_entity_get_parent(self_data, init_data->window_entity_name);

    init_data->renderer = SDL_CreateRenderer(parent_window->window, -1, init_data->flags);

    if (init_data->renderer) {
        tarasque_entity_subscribe_to_event(self_data, "sdl renderer pre draw",  (tarasque_specific_event_subscription) { .callback = &base_entity_sdl_render_manager_pre_draw });
        tarasque_entity_subscribe_to_event(self_data, "sdl renderer post draw", (tarasque_specific_event_subscription) { .callback = &base_entity_sdl_render_manager_post_draw });
    }
}

/**
 * @brief
 *
 * @param self_data
 */
static void base_entity_sdl_render_manager_deinit(tarasque_entity *self_data)
{
    if (!self_data) {
        return;
    }

    base_entity_sdl_render_manager_data *data = (base_entity_sdl_render_manager_data *) self_data;

    SDL_DestroyRenderer(data->renderer);
    data->renderer = NULL;
}

/**
 * @brief
 *
 * @param self_data
 * @param elapsed_ms
 */
static void base_entity_sdl_render_manager_on_frame(tarasque_entity *self_data, float elapsed_ms)
{
    if (!self_data) {
        return;
    }

    base_entity_sdl_render_manager_data *data = (base_entity_sdl_render_manager_data *) self_data;

    tarasque_entity_stack_event(self_data, "sdl renderer post draw", (tarasque_specific_event) { 0u });
    tarasque_entity_stack_event(self_data, "sdl renderer draw", (tarasque_specific_event) { .data_size = sizeof(base_entity_sdl_render_manager_event_draw), .data = &(base_entity_sdl_render_manager_event_draw) { data->renderer } });
    tarasque_entity_stack_event(self_data, "sdl renderer pre draw", (tarasque_specific_event) { 0u });
}

/**
 * @brief
 *
 * @param self_data
 * @param event_data
 */
static void base_entity_sdl_render_manager_pre_draw(tarasque_entity *self_data, void *event_data)
{
    (void) event_data;

    if (!self_data) {
        return;
    }

    base_entity_sdl_render_manager_data *data = (base_entity_sdl_render_manager_data *) self_data;

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
static void base_entity_sdl_render_manager_post_draw(tarasque_entity *self_data, void *event_data)
{
    (void) event_data;

    if (!self_data) {
        return;
    }

    base_entity_sdl_render_manager_data *data = (base_entity_sdl_render_manager_data *) self_data;

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
tarasque_specific_entity base_entity_sdl_render_manager(base_entity_sdl_render_manager_data *args)
{
    return (tarasque_specific_entity) {
            .data_size = sizeof(*args),
            .data = args,

            .callbacks = {
                    .on_init = &base_entity_sdl_render_manager_init,
                    .on_frame = &base_entity_sdl_render_manager_on_frame,
                    .on_deinit = &base_entity_sdl_render_manager_deinit,
            }
    };
}
