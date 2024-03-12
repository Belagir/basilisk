
#ifndef __SDL2_WINDOW_H__
#define __SDL2_WINDOW_H__

#include <SDL2/SDL.h>

#include <tarasque.h>

typedef struct graft_sdl_window_args { const char *title; int width; int height; uint32_t flags; } graft_sdl_window_args;

void graft_sdl_window(tarasque_engine *handle, const char *path, graft_sdl_window_args args);

#endif
