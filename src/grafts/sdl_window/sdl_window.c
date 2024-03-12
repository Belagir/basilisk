
#include <SDL2/SDL.h>

#include <grafts/sdl2_window.h>

#include "../entities/graft_entities.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

void graft_sdl_window(tarasque_engine *handle, const char *path, graft_sdl_window_args args)
{
    tarasque_engine_add_entity(handle, "", "SDL Context", graft_entity_sdl_context);
    tarasque_engine_add_entity(handle, "SDL Context", "SDL Window", graft_entity_sdl_window);
    tarasque_engine_add_entity(handle, "SDL Context/SDL Window", "SDL Event Relay", graft_entity_sdl_event_relay);
    tarasque_engine_add_entity(handle, "SDL Context/SDL Window", "SDL Render Manager", graft_entity_sdl_render_manager);
}
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
