
#include <SDL2/SDL.h>

#include <grafts/sdl2_window.h>

void graft_sdl_window(tarasque_entity *entity, void *graft_args)
{
    if (!graft_args) {
        return;
    }

    const graft_sdl_window_args sdl_win_args = *(const graft_sdl_window_args *) graft_args;

    tarasque_entity_add_child(entity, "", "SDL Context",
            be_sdl_context());

    tarasque_entity_add_child(entity, "SDL Context", "SDL Window",
            be_sdl_window(&(be_sdl_window_data) {
                    .title = sdl_win_args.for_window.title,
                    .x = sdl_win_args.for_window.x, .y = sdl_win_args.for_window.y,
                    .w = sdl_win_args.for_window.w, .h = sdl_win_args.for_window.h,
                    .flags = sdl_win_args.for_window.flags }));

    tarasque_entity_add_child(entity, "SDL Context/SDL Window", "SDL Render Manager",
            be_sdl_render_manager(&(be_sdl_render_manager_data) {
                    .clear_color = sdl_win_args.for_renderer.clear_color,
                    .flags = sdl_win_args.for_renderer.flags,
                    .window_entity_name = "SDL Window" }));

    tarasque_entity_add_child(entity, "SDL Context/SDL Window", "SDL Event Relay",
            be_sdl_event_relay(&(be_sdl_event_relay_data) { 0u }));
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
