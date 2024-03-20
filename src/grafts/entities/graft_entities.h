
#ifndef __SDL2_WINDOW_ENTITIES_H__
#define __SDL2_WINDOW_ENTITIES_H__

#include <SDL2/SDL.h>

#include <tarasque.h>
#include <grafts/sdl2_window.h>

// -------------------------------------------------------------------------------------------------

/*  */
tarasque_entity_specific_data graft_entity_sdl_context(void);

// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 */
typedef struct graft_entity_sdl_window_args {
    graft_sdl_window_win_args for_window;
    graft_sdl_window_render_args for_render;
} graft_entity_sdl_window_args;
/*  */
tarasque_entity_specific_data graft_entity_sdl_window(graft_entity_sdl_window_args args);

// -------------------------------------------------------------------------------------------------

/*  */
tarasque_entity_specific_data graft_entity_sdl_event_relay(void);

// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 */
typedef struct graft_entity_sdl_render_manager_args {
    graft_sdl_window_render_args args;
    SDL_Window *source_window;
} graft_entity_sdl_render_manager_args;
/*  */
tarasque_entity_specific_data graft_entity_sdl_render_manager(graft_entity_sdl_render_manager_args args);

// -------------------------------------------------------------------------------------------------

#endif
