
#include <SDL2/SDL.h>

#include <grafts/sdl2_window.h>

#include <base_entities/sdl_context.h>
#include <base_entities/sdl_event_relay.h>
#include <base_entities/sdl_render_manager.h>
#include <base_entities/sdl_window.h>

void graft_sdl_window(tarasque_entity *entity, void *graft_args)
{
    if (!graft_args) {
        return;
    }

    const graft_sdl_window_args sdl_win_args = *(const graft_sdl_window_args *) graft_args;

    tarasque_entity_add_child(entity, "", "SDL Context",
            base_entity_sdl_context());

    tarasque_entity_add_child(entity, "SDL Context", "SDL Window",
            base_entity_sdl_window(&(base_entity_sdl_window_data) {
                    .title = sdl_win_args.for_window.title,
                    .x = sdl_win_args.for_window.x, .y = sdl_win_args.for_window.y,
                    .w = sdl_win_args.for_window.w, .h = sdl_win_args.for_window.h,
                    .flags = sdl_win_args.for_window.flags }));

    tarasque_entity_add_child(entity, "SDL Context/SDL Window", "SDL Event Relay",
            base_entity_sdl_event_relay());
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
