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

    if (!graft_args) {
        return;
    }

    const graft_sdl_window_args sdl_win_args = *(const graft_sdl_window_args *) graft_args;

    context = tarasque_entity_add_child(entity, "Context", be_context_sdl_entity());

    window = tarasque_entity_add_child(context, "Window",
            be_window_sdl_entity(&(be_window_sdl) {
                    .title = sdl_win_args.for_window.title,
                    .x = sdl_win_args.for_window.x, .y = sdl_win_args.for_window.y,
                    .w = sdl_win_args.for_window.w, .h = sdl_win_args.for_window.h,
                    .flags = sdl_win_args.for_window.flags }));

    render_manager = tarasque_entity_add_child(window, "Render Manager",
            be_render_manager_sdl_entity(&(be_render_manager_sdl) {
                    .clear_color = sdl_win_args.for_renderer.clear_color,
                    .flags = sdl_win_args.for_renderer.flags,
                    .window_entity_name = "Window" }));

    event_relay = tarasque_entity_add_child(window, "Event Relay", be_event_relay_sdl_entity(&(be_event_relay_sdl) { 0u }));
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