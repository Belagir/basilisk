
#include <SDL2/SDL.h>

#include "graft_entities.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/*  */
static void graft_entity_sdl_context_init(tarasque_entity *self_data);
/*  */
static void graft_entity_sdl_context_deinit(tarasque_entity *self_data);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param self_data
 * @param scene
 */
static void graft_entity_sdl_context_init(tarasque_entity *self_data)
{
    (void) self_data;

    SDL_Init(SDL_INIT_EVERYTHING);
}

/**
 * @brief
 *
 * @param self_data
 * @param scene
 */
static void graft_entity_sdl_context_deinit(tarasque_entity *self_data)
{
    (void) self_data;

    if (SDL_WasInit(0)) {
        SDL_Quit();
    }
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

tarasque_entity_specific_data graft_entity_sdl_context(void)
{
    return (tarasque_entity_specific_data) {
        .callbacks = {
                .on_init = &graft_entity_sdl_context_init,
                .on_deinit = &graft_entity_sdl_context_deinit,
        }
    };
}

