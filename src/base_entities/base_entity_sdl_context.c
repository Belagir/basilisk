
#include <SDL2/SDL.h>

#include <base_entities/sdl_context.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/*  */
static void base_entity_sdl_context_init(tarasque_entity *self_data);
/*  */
static void base_entity_sdl_context_deinit(tarasque_entity *self_data);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param self_data
 * @param scene
 */
static void base_entity_sdl_context_init(tarasque_entity *self_data)
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
static void base_entity_sdl_context_deinit(tarasque_entity *self_data)
{
    (void) self_data;

    if (SDL_WasInit(0)) {
        SDL_Quit();
    }
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

tarasque_specific_entity base_entity_sdl_context(void)
{
    return (tarasque_specific_entity) {
        .callbacks = {
                .on_init = &base_entity_sdl_context_init,
                .on_deinit = &base_entity_sdl_context_deinit,
        }
    };
}

