
#ifndef __BASE_ENTITY_SDL_RENDER_MANAGER_H__
#define __BASE_ENTITY_SDL_RENDER_MANAGER_H__

#include <SDL2/SDL.h>
#include <tarasque.h>

/**
 * @brief
 *
 */
typedef struct be_sdl_render_manager_ev_draw {
    SDL_Renderer *renderer;
} be_sdl_render_manager_ev_draw;


/**
 * @brief
 *
 */
typedef struct be_sdl_render_manager_data {
    SDL_Color clear_color;
    SDL_RendererFlags flags;
    const char *window_entity_name;

    SDL_Renderer *renderer;
} be_sdl_render_manager_data;

tarasque_specific_entity be_sdl_render_manager(be_sdl_render_manager_data *args);

#endif
