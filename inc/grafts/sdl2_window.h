
#ifndef __SDL2_WINDOW_H__
#define __SDL2_WINDOW_H__

#include <SDL2/SDL.h>

#include <tarasque.h>

typedef struct graft_sdl_window_event_draw {
    SDL_Renderer *renderer;
} graft_sdl_window_event_draw;


typedef struct graft_sdl_window_args {
    const char *title;
    size_t x, y;
    size_t w, h;
    SDL_WindowFlags win_flags;

    // TODO : inject those to the window --> renderer
    SDL_RendererFlags renderer_flags;
    SDL_Color clear_color;
} graft_sdl_window_args;

typedef enum graft_sdl_window_args_size { GRAFT_SDL_WINDOW_ARGS_SIZE = sizeof(graft_sdl_window_args) } graft_sdl_window_args_size;

void graft_sdl_window(entity_data *entity, void *graft_args);

#endif
