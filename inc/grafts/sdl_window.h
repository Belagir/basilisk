/**
 * @file sdl_window.h
 * @author gabriel ()
 * @brief Header file of the graft adding an SDL window to the game tree.
 * The graft is made to provide a lightweight framework to generate a window, draw on it, and receive events from the library.
 *
 * @version 0.1
 * @date 2024-03-23
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef __SDL2_WINDOW_H__
#define __SDL2_WINDOW_H__

#include <SDL2/SDL.h>

#include <tarasque.h>

// Importing base entities included in the graft.

#include <base_entities/sdl_context.h>
#include <base_entities/sdl_event_relay.h>
#include <base_entities/sdl_render_manager.h>
#include <base_entities/sdl_window.h>

/**
 * @brief Arguments passed to the SDL window graft callback.
 */
typedef struct graft_sdl_window_args {
    /** Configuration given to the window entity. */
    struct {
        const char *title;
        size_t x, y;
        size_t w, h;
        SDL_WindowFlags flags;
    } for_window;

    /** Configuration given to the render manager entity. */
    struct {
        SDL_RendererFlags flags;
        SDL_Color clear_color;
    } for_renderer;
} graft_sdl_window_args;

/* Generates a graft configuration for the engine with arguments given from the callsite. */
tarasque_specific_graft graft_sdl_window(graft_sdl_window_args *args);

#endif
