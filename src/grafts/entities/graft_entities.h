
#ifndef __SDL2_WINDOW_ENTITIES_H__
#define __SDL2_WINDOW_ENTITIES_H__

#include <SDL2/SDL.h>
#include <tarasque.h>

/*  */
entity_user_data graft_entity_sdl_context(void);

/*  */
typedef struct graft_entity_sdl_window_args { const char *title; size_t x; size_t y; size_t w; size_t h; SDL_WindowFlags flags; } graft_entity_sdl_window_args;
/*  */
entity_user_data graft_entity_sdl_window(graft_entity_sdl_window_args args);

/*  */
entity_user_data graft_entity_sdl_event_relay(void);

/*  */
typedef struct graft_entity_sdl_render_manager_args { SDL_RendererFlags flags; SDL_Window *source_window; } graft_entity_sdl_render_manager_args;
/*  */
entity_user_data graft_entity_sdl_render_manager(graft_entity_sdl_render_manager_args args);

#endif
