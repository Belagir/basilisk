
#ifndef __BASE_ENTITY_SDL_RENDER_MANAGER_H__
#define __BASE_ENTITY_SDL_RENDER_MANAGER_H__

#include <SDL2/SDL.h>
#include <tarasque.h>

/**
 * @brief
 *
 */
typedef struct base_entity_sdl_render_manager_event_draw {
    SDL_Renderer *renderer;
} base_entity_sdl_render_manager_event_draw;


/**
 * @brief
 *
 */
typedef struct base_entity_sdl_render_manager_data {
    SDL_Color clear_color;
    SDL_RendererFlags flags;
    const char *window_entity_name;

    SDL_Renderer *renderer;
} base_entity_sdl_render_manager_data;

tarasque_specific_entity base_entity_sdl_render_manager(base_entity_sdl_render_manager_data *args);

#endif
