
#include <SDL2/SDL.h>

#include <base_entities/sdl_entities.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/*  */
static void be_context_sdl_init(tarasque_entity *self_data);
/*  */
static void be_context_sdl_deinit(tarasque_entity *self_data);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param self_data
 * @param scene
 */
static void be_context_sdl_init(tarasque_entity *self_data)
{
    (void) self_data;

    SDL_Init(SDL_INIT_TIMER);
}

/**
 * @brief
 *
 * @param self_data
 * @param scene
 */
static void be_context_sdl_deinit(tarasque_entity *self_data)
{
    (void) self_data;

    if (SDL_WasInit(0)) {
        SDL_Quit();
    }
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

tarasque_specific_entity BE_context_sdl_entity(void)
{
    return (tarasque_specific_entity) {
        .callbacks = {
                .on_init = &be_context_sdl_init,
                .on_deinit = &be_context_sdl_deinit,
        }
    };
}

