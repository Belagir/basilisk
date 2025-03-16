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
static void BE_context_sdl_init(basilisk_entity *self_data);
/* Quits the SDL library instance. */
static void BE_context_sdl_deinit(basilisk_entity *self_data);

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
static void BE_context_sdl_init(basilisk_entity *self_data)
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
static void BE_context_sdl_deinit(basilisk_entity *self_data)
{
    (void) self_data;

    if (SDL_WasInit(0)) {
        SDL_Quit();
    }
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------


// -------------------------------------------------------------------------------------------------

/**
 * @brief Defines the entity properties of a BE_context_sdl entity.
 *
 * This entity is here to provide a clear lifetime for any SDL-dependent entities. Some may not need this entity loaded to
 * function (because they call `SDL_InitSubSystem()`) but putting those as a child of a SDL Context entity leads to a more
 * comprehensible hierarchy.
 *
 */
const basilisk_entity_definition ENTITY_DEF_CONTEXT_SDL = {
        .on_init = &BE_context_sdl_init,
        .on_deinit = &BE_context_sdl_deinit,
};

struct basilisk_specific_entity create_context_sdl(void)
{
    return (struct basilisk_specific_entity) {
            .entity_def = ENTITY_DEF_CONTEXT_SDL,
            .data = nullptr,
    };
}
