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
static basilisk_entity *graft_sdl_window_callback(basilisk_entity *entity, void *graft_args);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static basilisk_entity *graft_sdl_window_callback(basilisk_entity *entity, void *graft_args)
{
    basilisk_entity *context = nullptr;
    basilisk_entity *window = nullptr;
    basilisk_entity *render_manager = nullptr;
    basilisk_entity *event_relay = nullptr;
    basilisk_entity *collision_manager = nullptr;

    if (!graft_args) {
        return nullptr;
    }

    const graft_sdl_window_args sdl_win_args = *(const graft_sdl_window_args *) graft_args;

    context = basilisk_entity_add_child(entity, "Context", create_context_sdl());

    window = basilisk_entity_add_child(context, "Window", create_window_sdl(
            sdl_win_args.for_window.title,
            sdl_win_args.for_window.w, sdl_win_args.for_window.h,
            sdl_win_args.for_window.x, sdl_win_args.for_window.y,
            sdl_win_args.for_window.flags));

    event_relay = basilisk_entity_add_child(window, "Event Relay", create_event_relay_sdl());

    render_manager = basilisk_entity_add_child(window, "Render Manager", create_render_manager_sdl(
            sdl_win_args.for_renderer.clear_color,
            sdl_win_args.for_renderer.w, sdl_win_args.for_renderer.h,
            sdl_win_args.for_renderer.flags));

    collision_manager = basilisk_entity_add_child(render_manager, "Collision Manager", create_collision_manager_2D());

    return render_manager;
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