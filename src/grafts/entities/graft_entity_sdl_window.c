
#include <SDL2/SDL.h>

#include "graft_entities.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 */
typedef struct graft_entity_sdl_window_data {

    SDL_Window *window;
} graft_entity_sdl_window_data;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/*  */
static void graft_entity_sdl_window_init(void *self_data, tarasque_entity_scene *scene);
/*  */
static void graft_entity_sdl_window_deinit(void *self_data, tarasque_entity_scene *scene);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static const entity_user_data global_graft_entity_sdl_window = {
        .data_size = sizeof(graft_entity_sdl_window_data),
        .data = &(graft_entity_sdl_window_data) { 0u },

        .on_init = graft_entity_sdl_window_init,
        .on_deinit = graft_entity_sdl_window_deinit,
};

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param self_data
 * @param scene
 */
static void graft_entity_sdl_window_init(void *self_data, tarasque_entity_scene *scene)
{
    (void) scene;

    graft_entity_sdl_window_data *window_data = (graft_entity_sdl_window_data *) self_data;
    if (!window_data) {
        return;
    }

    // TODO : inject configuration to this call
    window_data->window = SDL_CreateWindow("window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1200, 800, 0);
}

/**
 * @brief
 *
 * @param self_data
 * @param scene
 */
static void graft_entity_sdl_window_deinit(void *self_data, tarasque_entity_scene *scene)
{
    (void) scene;

    graft_entity_sdl_window_data *window_data = (graft_entity_sdl_window_data *) self_data;
    if (!window_data) {
        return;
    }

    SDL_DestroyWindow(window_data->window);
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

entity_user_data *graft_entity_sdl_window_as(const char *title, size_t x, size_t y,  size_t w, size_t h, SDL_WindowFlags flags)
{
    if (!title) {
        title = "window";
    }

}
