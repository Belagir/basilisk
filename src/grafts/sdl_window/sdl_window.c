
#include <SDL2/SDL.h>

#include <grafts/sdl2_window.h>

#include "../entities/graft_entities.h"

void graft_sdl_window(entity_data *entity, void *graft_args)
{
    if (!graft_args) {
        return;
    }

    const graft_sdl_window_args sdl_win_args = *(const graft_sdl_window_args *) graft_args;

    tarasque_entity_add_child(entity, "", "SDL Context",
            graft_entity_sdl_context());

    tarasque_entity_add_child(entity, "SDL Context", "SDL Window",
            graft_entity_sdl_window((graft_entity_sdl_window_args) {
                    sdl_win_args.title,
                    sdl_win_args.x, sdl_win_args.y,
                    sdl_win_args.w, sdl_win_args.h,
                    sdl_win_args.win_flags }));

    tarasque_entity_add_child(entity, "SDL Context/SDL Window", "SDL Event Relay",
            graft_entity_sdl_event_relay());
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
