
#include <SDL2/SDL.h>

#include "graft_entities.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/*  */
static void graft_entity_sdl_context_init(void *self_data, tarasque_engine *handle);
/*  */
static void graft_entity_sdl_context_deinit(void *self_data, tarasque_engine *handle);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param self_data
 * @param handle
 */
static void graft_entity_sdl_context_init(void *self_data, tarasque_engine *handle)
{
    (void) self_data;

    SDL_Init(SDL_INIT_EVERYTHING);
}

/**
 * @brief
 *
 * @param self_data
 * @param handle
 */
static void graft_entity_sdl_context_deinit(void *self_data, tarasque_engine *handle)
{
    (void) self_data;

    if (SDL_WasInit(0)) {
        SDL_Quit();
    }
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

const entity_user_data graft_entity_sdl_context = {
        .on_init = &graft_entity_sdl_context_init,
        .on_deinit = &graft_entity_sdl_context_deinit,
};
