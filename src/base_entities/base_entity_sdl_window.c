
#include <base_entities/sdl_entities.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/*  */
static void BE_window_sdl_init(tarasque_entity *self_data);
/*  */
static void BE_window_sdl_deinit(tarasque_entity *self_data);
/*  */
static void BE_window_sdl_quit(tarasque_entity *self_data, void *event_data);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param self_data
 * @param scene
 */
static void BE_window_sdl_init(tarasque_entity *self_data)
{
    if (!self_data) {
        return;
    }

    BE_window_sdl *window_data = (BE_window_sdl *) self_data;

    SDL_InitSubSystem(SDL_INIT_VIDEO);

    window_data->window = SDL_CreateWindow(
            window_data->title,
            (int) window_data->x, (int) window_data->y,
            (int) window_data->w, (int) window_data->h,
            window_data->flags);

    tarasque_entity_queue_subscribe_to_event(self_data, "sdl event quit", (tarasque_specific_event_subscription) { .callback = &BE_window_sdl_quit });
}

/**
 * @brief
 *
 * @param self_data
 * @param scene
 */
static void BE_window_sdl_deinit(tarasque_entity *self_data)
{
    if (!self_data) {
        return;
    }

    BE_window_sdl *window_data = (BE_window_sdl *) self_data;

    SDL_DestroyWindow(window_data->window);
    window_data->window = NULL;
}

/**
 * @brief
 *
 * @param self_data
 * @param event_data
 */
static void BE_window_sdl_quit(tarasque_entity *self_data, void *event_data)
{
    SDL_QuitSubSystem(SDL_INIT_VIDEO);
    tarasque_entity_queue_remove(self_data);
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

tarasque_entity_definition BE_window_sdl_entity(void)
{
    return (tarasque_entity_definition) {
            .data_size = sizeof(BE_window_sdl),

            .on_init = &BE_window_sdl_init,
            .on_deinit = &BE_window_sdl_deinit,
    };
}