
#include <SDL2/SDL.h>

#include <grafts/sdl2_window.h>

#include "../entities/graft_entities.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/*  */
static void graft_sdl2_window_init(void *self_data, tarasque_engine *handle);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param self_data
 * @param handle
 */
static void graft_sdl2_window_init(void *self_data, tarasque_engine *handle)
{
    (void) self_data;

    // adding context, window & others
    tarasque_engine_add_entity(handle, "", "SDL Context", graft_entity_sdl_context);
    tarasque_engine_add_entity(handle, "SDL Context", "SDL Window", graft_entity_sdl_window);
    tarasque_engine_add_entity(handle, "SDL Context/SDL Window", "SDL Event Relay", graft_entity_sdl_event_relay);
    tarasque_engine_add_entity(handle, "SDL Context/SDL Window", "SDL Render Manager", graft_entity_sdl_render_manager);
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

const entity_user_data graft_sdl2_window = {
        .on_init = &graft_sdl2_window_init,
};
