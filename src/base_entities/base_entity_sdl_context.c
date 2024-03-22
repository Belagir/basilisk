
#include <SDL2/SDL.h>

#include <base_entities/sdl_context.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/*  */
static void be_sdl_context_init(tarasque_entity *self_data);
/*  */
static void be_sdl_context_deinit(tarasque_entity *self_data);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param self_data
 * @param scene
 */
static void be_sdl_context_init(tarasque_entity *self_data)
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
static void be_sdl_context_deinit(tarasque_entity *self_data)
{
    (void) self_data;

    if (SDL_WasInit(0)) {
        SDL_Quit();
    }
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

tarasque_specific_entity be_sdl_context(void)
{
    return (tarasque_specific_entity) {
        .callbacks = {
                .on_init = &be_sdl_context_init,
                .on_deinit = &be_sdl_context_deinit,
        }
    };
}

