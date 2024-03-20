
#include <SDL2/SDL.h>

#include "graft_entities.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/*  */
static void graft_entity_sdl_context_init(entity_data *self_data);
/*  */
static void graft_entity_sdl_context_deinit(entity_data *self_data);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param self_data
 * @param scene
 */
static void graft_entity_sdl_context_init(entity_data *self_data)
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
static void graft_entity_sdl_context_deinit(entity_data *self_data)
{
    (void) self_data;

    if (SDL_WasInit(0)) {
        SDL_Quit();
    }
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

entity_user_data graft_entity_sdl_context(void)
{
    return (entity_user_data) {
        .callbacks = {
                .on_init = &graft_entity_sdl_context_init,
                .on_deinit = &graft_entity_sdl_context_deinit,
        }
    };
}

