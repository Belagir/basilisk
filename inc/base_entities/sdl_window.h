
#ifndef __BASE_ENTITY_SDL_WINDOW_H__
#define __BASE_ENTITY_SDL_WINDOW_H__

#include <SDL2/SDL.h>

#include <tarasque.h>

/**
 * @brief
 *
 */
typedef struct base_entity_sdl_window_data {
    const char *title;
    size_t x, y;
    size_t w, h;
    SDL_WindowFlags flags;

    SDL_Window *window;
} base_entity_sdl_window_data;

tarasque_specific_entity base_entity_sdl_window(base_entity_sdl_window_data *args);

#endif
