
#include <base_entities/sdl_entities.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

typedef struct BE_window_sdl {
    /** Title of the window used on window creation. */
    const char *title;
    /** Pixel position of the window on creation. */
    size_t x, y;
    /** Pixel position of the window on creation. */
    size_t w, h;
    /** Flags passed to the SDL window creation method on initialization of the entity. See the SDL_WindowFlags enum for values. */
    SDL_WindowFlags flags;

    /** Pointer to a window that will be created on entity initialization. Overriden on initialization. */
    SDL_Window *window;
} BE_window_sdl;

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

/**
 * @brief
 *
 * @param title
 * @param w
 * @param h
 * @param x
 * @param y
 * @param flags
 * @return
 */
tarasque_entity *BE_STATIC_window_sdl(const char *title, size_t w, size_t h, size_t x, size_t y, SDL_WindowFlags flags)
{
    static BE_window_sdl buffer = { 0u };

    buffer = (BE_window_sdl) {
            .title = title,
            .x = x, .y = y,
            .w = w, .h = h,
            .flags = flags,
    };

    return &buffer;
}

/**
 * @brief
 *
 * @param window_data
 * @return
 */
SDL_Window *BE_window_sdl_get_window(BE_window_sdl *window_entity)
{
    if (!window_entity) {
        return NULL;
    }

    return window_entity->window;
}

// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 */
const tarasque_entity_definition BE_DEF_window_sdl = {
        .data_size = sizeof(BE_window_sdl),

        .on_init = &BE_window_sdl_init,
        .on_deinit = &BE_window_sdl_deinit,
};