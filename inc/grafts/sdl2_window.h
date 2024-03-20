
#ifndef __SDL2_WINDOW_H__
#define __SDL2_WINDOW_H__

#include <SDL2/SDL.h>

#include <tarasque.h>

/**
 * @brief
 *
 */
typedef struct graft_sdl_window_event_draw {
    SDL_Renderer *renderer;
} graft_sdl_window_event_draw;

/**
 * @brief
 *
 */
typedef struct graft_sdl_window_win_args {
    const char *title;
    size_t x, y;
    size_t w, h;
    SDL_WindowFlags flags;
} graft_sdl_window_win_args;

/**
 * @brief
 *
 */
typedef struct graft_sdl_window_render_args {
    SDL_RendererFlags flags;
    SDL_Color clear_color;
} graft_sdl_window_render_args;

/**
 * @brief
 *
 */
typedef struct graft_sdl_window_args {
    graft_sdl_window_win_args for_window;
    graft_sdl_window_render_args for_renderer;
} graft_sdl_window_args;

/**
 * @brief
 *
 */
typedef enum graft_sdl_window_args_size { GRAFT_SDL_WINDOW_ARGS_SIZE = sizeof(graft_sdl_window_args) } graft_sdl_window_args_size;

/*  */
void graft_sdl_window(tarasque_entity *entity, void *graft_args);

#endif
