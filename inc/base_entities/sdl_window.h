
#ifndef __BASE_ENTITY_SDL_WINDOW_H__
#define __BASE_ENTITY_SDL_WINDOW_H__

#include <SDL2/SDL.h>

#include <tarasque_bare.h>

/**
 * @brief
 *
 */
typedef struct be_window_sdl {
    const char *title;
    size_t x, y;
    size_t w, h;
    SDL_WindowFlags flags;

    SDL_Window *window;
} be_window_sdl;

tarasque_specific_entity be_window_sdl_entity(be_window_sdl *args);

#endif
