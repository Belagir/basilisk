
#ifndef __BASE_ENTITIES_SDL_ENTITIES_H__
#define __BASE_ENTITIES_SDL_ENTITIES_H__

#include <SDL2/SDL.h>

#include <ustd/math2d.h>

#include <tarasque.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

#define BE_EVENT_RELAY_SDL_BUFFER_SIZE (64)

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 */
typedef struct BE_event_relay_sdl {
    SDL_Event event_buffer[BE_EVENT_RELAY_SDL_BUFFER_SIZE];
} BE_event_relay_sdl;

/**
 * @brief
 *
 */
typedef struct BE_render_manager_sdl_event_draw {
    SDL_Renderer *renderer;
} BE_render_manager_sdl_event_draw;


/**
 * @brief
 *
 */
typedef struct BE_render_manager_sdl {
    SDL_Color clear_color;
    SDL_RendererFlags flags;
    const char *window_entity_name;

    SDL_Renderer *renderer;
} BE_render_manager_sdl;

/**
 * @brief
 *
 */
typedef struct BE_window_sdl {
    const char *title;
    size_t x, y;
    size_t w, h;
    SDL_WindowFlags flags;

    SDL_Window *window;
} BE_window_sdl;

// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 */
typedef struct properties_2D {
    vector2_t position;
    vector2_t scale;
    f32 angle;
} properties_2D;

/**
 * @brief
 *
 */
typedef struct BE_body_2D BE_body_2D;
typedef struct BE_body_2D {
    BE_body_2D *previous;

    properties_2D local;
    properties_2D global;
} BE_body_2D;

/**
 * @brief
 *
 */
typedef struct BE_texture_2D {
    BE_body_2D body;
    i32 draw_index;

    SDL_Texture *texture;
} BE_texture_2D;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* */
tarasque_specific_entity BE_context_sdl_entity(void);

/* */
tarasque_specific_entity BE_event_relay_sdl_entity(BE_event_relay_sdl *args);

/* */
tarasque_specific_entity BE_render_manager_sdl_entity(BE_render_manager_sdl *args);

/* */
tarasque_specific_entity BE_window_sdl_entity(BE_window_sdl *args);

// -------------------------------------------------------------------------------------------------

/* */
tarasque_specific_entity BE_body_2D_entity(BE_body_2D *args);

/* */
tarasque_specific_entity BE_texture_2D_entity(BE_texture_2D *args);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

#endif
