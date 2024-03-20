
#include <base_entities/sdl_window.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/*  */
static void base_entity_sdl_window_init(tarasque_entity *self_data);
/*  */
static void base_entity_sdl_window_deinit(tarasque_entity *self_data);
/*  */
static void base_entity_sdl_window_quit(tarasque_entity *self_data, void *event_data);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param self_data
 * @param scene
 */
static void base_entity_sdl_window_init(tarasque_entity *self_data)
{
    if (!self_data) {
        return;
    }

    base_entity_sdl_window_data *window_data = (base_entity_sdl_window_data *) self_data;

    SDL_InitSubSystem(SDL_INIT_VIDEO);

    window_data->window = SDL_CreateWindow(
            window_data->title,
            (int) window_data->x, (int) window_data->y,
            (int) window_data->w, (int) window_data->h,
            window_data->flags);

    tarasque_entity_subscribe_to_event(self_data, "sdl event quit", (tarasque_specific_event_subscription) { &base_entity_sdl_window_quit });
}

/**
 * @brief
 *
 * @param self_data
 * @param scene
 */
static void base_entity_sdl_window_deinit(tarasque_entity *self_data)
{
    if (!self_data) {
        return;
    }

    base_entity_sdl_window_data *window_data = (base_entity_sdl_window_data *) self_data;

    SDL_DestroyWindow(window_data->window);
    window_data->window = NULL;
}

static void base_entity_sdl_window_quit(tarasque_entity *self_data, void *event_data)
{
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    tarasque_entity_remove_child(self_data, "");
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

tarasque_specific_entity base_entity_sdl_window(base_entity_sdl_window_data *args)
{
    return (tarasque_specific_entity) {
            .data_size = sizeof(*args),
            .data = args,

            .callbacks = {
                    .on_init = &base_entity_sdl_window_init,
                    .on_deinit = &base_entity_sdl_window_deinit,
            }
    };
}