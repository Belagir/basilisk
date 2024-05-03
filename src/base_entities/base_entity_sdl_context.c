/**
 * @file base_entity_sdl_context.c
 * @author gabriel ()
 * @brief Implentation file for the SDL Context entity.
 * @version 0.1
 * @date 2024-04-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <SDL2/SDL.h>

#include <base_entities/sdl_entities.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* Initialise a minimal SDL library configuration. */
static void BE_context_sdl_init(tarasque_entity *self_data);
/* Quits the SDL library instance. */
static void BE_context_sdl_deinit(tarasque_entity *self_data);

// TODO : make the entity accept empty "log SDL Error" event

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief BE_context_sdl initialisation callback.
 * Initialise the entity and SDL context by calling `SDL_Init()` without module specified.
 *
 * @param[in] self_data discarded entity data
 */
static void BE_context_sdl_init(tarasque_entity *self_data)
{
    (void) self_data;

    SDL_Init(0u);
}

/**
 * @brief BE_context_sdl deinitialisation callback.
 * Quits the SDL context by calling `SDL_Quit()`.
 *
 * @param[in] self_data discarded entity data
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
 * @brief Returns a NULL object, because the BE_context_sdl entity does not need a memory object to be initialised.
 * This function is provided for coherence with other entities and to future proof against possible extentions to this entity.
 *
 * @see BE_DEF_context_sdl
 *
 * @return tarasque_entity *
 */
tarasque_entity *BE_STATIC_context_sdl(void)
{
    return NULL;
}

// -------------------------------------------------------------------------------------------------

/**
 * @brief Defines the entity properties of a BE_context_sdl entity.
 *
 * This entity is here to provide a clear lifetime for any SDL-dependent entities. Some may not need this entity loaded to
 * function (because they call `SDL_InitSubSystem()`) but putting those as a child of a SDL Context entity leads to a more
 * comprehensible hierarchy.
 *
 * @see BE_STATIC_context_sdl
 *
 */
const tarasque_entity_definition BE_DEF_context_sdl = {
        .on_init = &BE_context_sdl_init,
        .on_deinit = &BE_context_sdl_deinit,
};

