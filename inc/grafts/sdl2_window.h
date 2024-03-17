
#ifndef __SDL2_WINDOW_H__
#define __SDL2_WINDOW_H__

#include <SDL2/SDL.h>

#include <tarasque.h>

typedef struct graft_sdl_window_args {
    const char *title;
    size_t x, y;
    size_t w, h;
    SDL_WindowFlags win_flags;
    SDL_RendererFlags renderer_flags;
} graft_sdl_window_args;

void graft_sdl_window(tarasque_engine *handle, void *graft_args);

#endif
