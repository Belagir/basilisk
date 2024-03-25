
#include <base_entities/sdl_window.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/*  */
static void be_window_sdl_init(tarasque_entity *self_data);
/*  */
static void be_window_sdl_deinit(tarasque_entity *self_data);
/*  */
static void be_window_sdl_quit(tarasque_entity *self_data, void *event_data);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param self_data
 * @param scene
 */
static void be_window_sdl_init(tarasque_entity *self_data)
{
    if (!self_data) {
        return;
    }

    be_window_sdl *window_data = (be_window_sdl *) self_data;

    SDL_InitSubSystem(SDL_INIT_VIDEO);

    window_data->window = SDL_CreateWindow(
            window_data->title,
            (int) window_data->x, (int) window_data->y,
            (int) window_data->w, (int) window_data->h,
            window_data->flags);

    tarasque_entity_queue_subscribe_to_event(self_data, "sdl event quit", (tarasque_specific_event_subscription) { .callback = &be_window_sdl_quit });
}

/**
 * @brief
 *
 * @param self_data
 * @param scene
 */
static void be_window_sdl_deinit(tarasque_entity *self_data)
{
    if (!self_data) {
        return;
    }

    be_window_sdl *window_data = (be_window_sdl *) self_data;

    SDL_DestroyWindow(window_data->window);
    window_data->window = NULL;
}

static void be_window_sdl_quit(tarasque_entity *self_data, void *event_data)
{
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    tarasque_entity_queue_remove_child(self_data, "");
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

tarasque_specific_entity be_window_sdl_entity(be_window_sdl *args)
{
    return (tarasque_specific_entity) {
            .data_size = sizeof(*args),
            .data = args,

            .callbacks = {
                    .on_init = &be_window_sdl_init,
                    .on_deinit = &be_window_sdl_deinit,
            }
    };
}