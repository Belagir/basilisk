
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

typedef enum graft_sdl_window_args_size { GRAFT_SDL_WINDOW_ARGS_SIZE = sizeof(graft_sdl_window_args) } graft_sdl_window_args_size;

void graft_sdl_window(tarasque_entity_scene *scene, void *graft_args);

#endif
