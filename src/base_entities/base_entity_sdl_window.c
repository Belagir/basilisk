/**
 * @file base_entity_sdl_window.c
 * @author gabriel ()
 * @brief Implementation file for the BE_window_sdl entity.
 * @version 0.1
 * @date 2024-04-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <base_entities/sdl_entities.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Private data needed by a "SDL window" entity to work. It retains the initialisation values and a handle to the actual SDL Window.
 *
 * @see ENTITY_DEF_WINDOW_SDL
 */
typedef struct BE_window_sdl {
    /** Title of the window used on window creation. */
    const char *title;
    /** Pixel position of the window on creation. */
    size_t x, y;
    /** Pixel position of the window on creation. */
    size_t w, h;
    /** Flags passed to the SDL window creation method on initialisation of the entity. See the SDL_WindowFlags enum for values. */
    SDL_WindowFlags flags;

    /** Pointer to a window that will be created on entity initialisation. Overriden on initialisation. */
    SDL_Window *window;
} BE_window_sdl;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* Creates a new window. */
static void BE_window_sdl_init(basilisk_entity *self_data);
/* Closes the window associated with the SDL Window entity. */
static void BE_window_sdl_deinit(basilisk_entity *self_data);
/* Callback for the special "sdl event quit" used to remove this entity. See the SDL Event Relay entity (BE_sdl_event_relay). */
static void BE_window_sdl_quit(basilisk_entity *self_data, void *event_data);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Initialises a BE_window_sdl entity. It will create a new SDL_Window and store a pointer to it.
 *
 * @param[inout] self_data points to some SDL Window entity data
 */
static void BE_window_sdl_init(basilisk_entity *self_data)
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

    basilisk_entity_queue_subscribe_to_event(self_data, "sdl event quit", (basilisk_specific_event_subscription) { .callback = &BE_window_sdl_quit });
}

/**
 * @brief Releases resources taken by the entity and closes the SDL Window it might be storing a pointer to.
 *
 * @param[inout] self_data pointer to a BE_window_sdl object
 */
static void BE_window_sdl_deinit(basilisk_entity *self_data)
{
    if (!self_data) {
        return;
    }

    BE_window_sdl *window_data = (BE_window_sdl *) self_data;

    SDL_DestroyWindow(window_data->window);
    window_data->window = nullptr;

    SDL_QuitSubSystem(SDL_INIT_VIDEO);
}

/**
 * @brief Callabck to quit the window. This will simply queue the removal of itself.
 *
 * @param[inout] self_data pointer to a BE_window_sdl object
 */
static void BE_window_sdl_quit(basilisk_entity *self_data, void *event_data)
{
    basilisk_entity_queue_remove(self_data);
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Returns the pointer to the SDL Window that the entity is managing.
 *
 * @param[in] window_data pointer to some SDL Window entity
 * @return SDL_Window *
 */
SDL_Window *BE_window_sdl_get_window(basilisk_entity *window_entity)
{
    if (!basilisk_entity_is(window_entity, ENTITY_DEF_WINDOW_SDL)) {
        return nullptr;
    }

    return ((struct BE_window_sdl *) window_entity)->window;
}

// -------------------------------------------------------------------------------------------------

/**
 * @brief Defines an entity to manage a handle of SDL Window that will match the lifetime of the entity.
 *
 * @see BE_window_sdl
 *
 * It might be a child of a BE_sdl_context entity.
 */
const basilisk_entity_definition ENTITY_DEF_WINDOW_SDL = {
        .data_size = sizeof(BE_window_sdl),

        .on_init = &BE_window_sdl_init,
        .on_deinit = &BE_window_sdl_deinit,
};

struct basilisk_specific_entity create_window_sdl(const char *title, size_t w, size_t h, size_t x, size_t y, SDL_WindowFlags flags)
{
    static BE_window_sdl buffer = { 0u };

    buffer = (BE_window_sdl) { 0u };
    buffer = (BE_window_sdl) {
            .title = title,
            .x = x, .y = y,
            .w = w, .h = h,
            .flags = flags,
    };

    return (struct basilisk_specific_entity) {
            .entity_def = ENTITY_DEF_WINDOW_SDL,
            .data = &buffer
    };
}