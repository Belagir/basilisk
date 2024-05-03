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
static void graft_sdl_window_callback(tarasque_entity *entity, void *graft_args);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static void graft_sdl_window_callback(tarasque_entity *entity, void *graft_args)
{
    tarasque_entity *context = NULL;
    tarasque_entity *window = NULL;
    tarasque_entity *render_manager = NULL;
    tarasque_entity *event_relay = NULL;
    tarasque_entity *collision_manager = NULL;

    if (!graft_args) {
        return;
    }

    const graft_sdl_window_args sdl_win_args = *(const graft_sdl_window_args *) graft_args;

    context = tarasque_entity_add_child(entity, "Context", (tarasque_specific_entity) {
            .entity_def = BE_DEF_context_sdl,
            .data = BE_STATIC_context_sdl() });

    window = tarasque_entity_add_child(context, "Window", (tarasque_specific_entity) {
            .entity_def = BE_DEF_window_sdl,
            .data = BE_STATIC_window_sdl(sdl_win_args.for_window.title, sdl_win_args.for_window.w, sdl_win_args.for_window.h, sdl_win_args.for_window.x, sdl_win_args.for_window.y, sdl_win_args.for_window.flags) });

    event_relay = tarasque_entity_add_child(window, "Event Relay", (tarasque_specific_entity) {
            .entity_def = BE_DEF_event_relay_sdl,
            .data = BE_STATIC_event_relay_sdl() });

    render_manager = tarasque_entity_add_child(window, "Render Manager", (tarasque_specific_entity) {
            .entity_def = BE_DEF_render_manager_sdl,
            .data = BE_STATIC_render_manager_sdl(sdl_win_args.for_renderer.clear_color, sdl_win_args.for_renderer.w, sdl_win_args.for_renderer.h, sdl_win_args.for_renderer.flags) });

    collision_manager = tarasque_entity_add_child(render_manager, "Collision Manager", (tarasque_specific_entity) { .entity_def = BE_DEF_collision_manager_2D, });
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Creates a graft configuration to pass to the engine to execute the graft.
 *
 * @param[inout] args
 * @return tarasque_specific_graft
 */
tarasque_specific_graft graft_sdl_window(graft_sdl_window_args *args)
{
        return (tarasque_specific_graft) {
                .args = args,
				.args_size = sizeof(*args),
				.graft_procedure = &graft_sdl_window_callback,
        };
}