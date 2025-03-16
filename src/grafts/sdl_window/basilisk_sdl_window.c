/**
 * @file sdl_window.c
 * @author gabriel ()
 * @brief Implementation file for the SDL window graft procedure.
 * @version 0.1
 * @date 2024-03-23
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <SDL2/SDL.h>

#include <grafts/sdl_window.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* Executes the actual graft by adding commands to the engine. */
static void graft_sdl_window_callback(basilisk_entity *entity, void *graft_args);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static void graft_sdl_window_callback(basilisk_entity *entity, void *graft_args)
{
    basilisk_entity *context = NULL;
    basilisk_entity *window = NULL;
    basilisk_entity *render_manager = NULL;
    basilisk_entity *event_relay = NULL;
    basilisk_entity *collision_manager = NULL;

    if (!graft_args) {
        return;
    }

    const graft_sdl_window_args sdl_win_args = *(const graft_sdl_window_args *) graft_args;

    context = basilisk_entity_add_child(entity, "Context", BE_CREATE_context_sdl());

    window = basilisk_entity_add_child(context, "Window", BE_CREATE_window_sdl(
            sdl_win_args.for_window.title,
            sdl_win_args.for_window.w, sdl_win_args.for_window.h,
            sdl_win_args.for_window.x, sdl_win_args.for_window.y,
            sdl_win_args.for_window.flags));

    event_relay = basilisk_entity_add_child(window, "Event Relay", BE_CREATE_event_relay_sdl());

    render_manager = basilisk_entity_add_child(window, "Render Manager", BE_CREATE_render_manager_sdl(
            sdl_win_args.for_renderer.clear_color,
            sdl_win_args.for_renderer.w, sdl_win_args.for_renderer.h,
            sdl_win_args.for_renderer.flags));

    collision_manager = basilisk_entity_add_child(render_manager, "Collision Manager", BE_CREATE_collision_manager_2D());
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Creates a graft configuration to pass to the engine to execute the graft.
 *
 * @param[inout] args
 * @return basilisk_specific_graft
 */
basilisk_specific_graft graft_sdl_window(graft_sdl_window_args *args)
{
        return (basilisk_specific_graft) {
                .args = args,
				.args_size = sizeof(*args),
				.graft_procedure = &graft_sdl_window_callback,
        };
}