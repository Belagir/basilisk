
#include <SDL2/SDL.h>

#include <base_entities/sdl_entities.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/*  */
static void BE_context_sdl_init(tarasque_entity *self_data);
/*  */
static void BE_context_sdl_deinit(tarasque_entity *self_data);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param self_data
 * @param scene
 */
static void BE_context_sdl_init(tarasque_entity *self_data)
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
static void BE_context_sdl_deinit(tarasque_entity *self_data)
{
    (void) self_data;

    if (SDL_WasInit(0)) {
        SDL_Quit();
    }
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param
 * @return
 */
tarasque_entity *BE_STATIC_context_sdl(void)
{
    return NULL;
}

// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 */
const tarasque_entity_definition BE_DEF_context_sdl = {
        .on_init = &BE_context_sdl_init,
        .on_deinit = &BE_context_sdl_deinit,
};

