
#include <SDL2/SDL.h>

#include <grafts/sdl2_window.h>

#include "../entities/graft_entities.h"

void graft_sdl_window(tarasque_engine *handle, void *graft_args)
{
    if (!graft_args) {
        return;
    }

    const graft_sdl_window_args sdl_win_args = *(const graft_sdl_window_args *) graft_args;

    tarasque_engine_add_entity(handle, "", "SDL Context", graft_entity_sdl_context);
    tarasque_engine_add_entity(handle, "SDL Context", "SDL Window", graft_entity_sdl_window);
    tarasque_engine_add_entity(handle, "SDL Context/SDL Window", "SDL Event Relay", graft_entity_sdl_event_relay);
    tarasque_engine_add_entity(handle, "SDL Context/SDL Window", "Render Manager", graft_entity_sdl_render_manager);
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
