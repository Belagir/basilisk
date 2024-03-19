
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
    graft_entity_sdl_window_args init_args;

    SDL_Window *window;
} graft_entity_sdl_window_data;

static graft_entity_sdl_window_data sdl_window_data_buffer = { 0u };

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/*  */
static void graft_entity_sdl_window_init(void *self_data, tarasque_entity_scene *scene);
/*  */
static void graft_entity_sdl_window_deinit(void *self_data, tarasque_entity_scene *scene);
/*  */
static void graft_entity_sdl_window_quit(void *self_data, void *event_data, tarasque_entity_scene *scene);

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

    if (!self_data) {
        return;
    }

    graft_entity_sdl_window_data *window_data = (graft_entity_sdl_window_data *) self_data;

    window_data->window = SDL_CreateWindow(
            window_data->init_args.title,
            (int) window_data->init_args.x, (int) window_data->init_args.y,
            (int) window_data->init_args.w, (int) window_data->init_args.h,
            window_data->init_args.flags);

    tarasque_entity_scene_add_entity(scene, "", "Render Manager",
            graft_entity_sdl_render_manager((graft_entity_sdl_render_manager_args) { .source_window = window_data->window, .flags = 0 }));

    tarasque_entity_scene_subscribe_to_event(scene, "sdl event quit", &graft_entity_sdl_window_quit);
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

    if (!self_data) {
        return;
    }

    graft_entity_sdl_window_data *window_data = (graft_entity_sdl_window_data *) self_data;

    SDL_DestroyWindow(window_data->window);
    window_data->window = NULL;
}

static void graft_entity_sdl_window_quit(void *self_data, void *event_data, tarasque_entity_scene *scene)
{
    tarasque_entity_scene_remove_entity(scene, "");
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

entity_user_data graft_entity_sdl_window(graft_entity_sdl_window_args args)
{
    // avoiding stack allocation but still won't force the user to deal with the heap...
    sdl_window_data_buffer = (graft_entity_sdl_window_data) {
            .init_args = args,

            .window = NULL,
    };

    return (entity_user_data) {
            .data_size = sizeof(sdl_window_data_buffer),
            .data = &sdl_window_data_buffer,
            .on_init = &graft_entity_sdl_window_init,
            .on_deinit = &graft_entity_sdl_window_deinit,
    };
}