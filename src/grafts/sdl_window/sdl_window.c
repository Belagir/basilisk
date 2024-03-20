
#include <SDL2/SDL.h>

#include <grafts/sdl2_window.h>

void graft_sdl_window(tarasque_entity *entity, void *graft_args)
{
    if (!graft_args) {
        return;
    }

    const graft_sdl_window_args sdl_win_args = *(const graft_sdl_window_args *) graft_args;

//     tarasque_entity_add_child(entity, "", "SDL Context",
//             graft_entity_sdl_context());

//     tarasque_entity_add_child(entity, "SDL Context", "SDL Window",
//             graft_entity_sdl_window((graft_entity_sdl_window_args) {
//                     .for_window = sdl_win_args.for_window,
//                     .for_render = sdl_win_args.for_renderer, }));

//     tarasque_entity_add_child(entity, "SDL Context/SDL Window", "SDL Event Relay",
//             graft_entity_sdl_event_relay());
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
