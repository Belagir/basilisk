
#ifndef __BASE_ENTITY_SDL_RENDER_MANAGER_H__
#define __BASE_ENTITY_SDL_RENDER_MANAGER_H__

#include <SDL2/SDL.h>
#include <tarasque.h>

/**
 * @brief
 *
 */
typedef struct be_render_manager_sdl_event_draw {
    SDL_Renderer *renderer;
} be_render_manager_sdl_event_draw;


/**
 * @brief
 *
 */
typedef struct be_render_manager_sdl {
    SDL_Color clear_color;
    SDL_RendererFlags flags;
    const char *window_entity_name;

    SDL_Renderer *renderer;
} be_render_manager_sdl;

tarasque_specific_entity be_render_manager_sdl_entity(be_render_manager_sdl *args);

#endif
