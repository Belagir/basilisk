/**
 * @file sdl_entities.h
 * @author gabriel ()
 * @brief Aggregates a set of entities and their related data structure and interfaces to easely build an SDL-powered 2D game.
 * @version 0.1
 * @date 2024-03-29
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef __BASE_ENTITIES_SDL_ENTITIES_H__
#define __BASE_ENTITIES_SDL_ENTITIES_H__

#include <SDL2/SDL.h>
#include <ustd/math2d.h>
#include <tarasque.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Data layout of a "sdl renderer draw" drawing event companion data sent by the E_render_manager_sdl_entity_def entity.
 */
typedef struct BE_render_manager_sdl_event_draw {
    /** Renderer that is currently accepting drawing operations. */
    SDL_Renderer *renderer;
} BE_render_manager_sdl_event_draw;

/**
 * @brief Data layout of an "render manager" base entity. Use it to configure the clear color and flags on entity creation.
 */
typedef struct BE_render_manager_sdl {
    /** Color used to clear the screen each frame before each drawing operation. */
    SDL_Color clear_color;
    /** Flags passed to the SDL renderer creation method on initialization of the entity. */
    SDL_RendererFlags flags;

    /** Pointer to a renderer that will be created on entity initialization. Overriden on initialization. */
    SDL_Renderer *renderer;
} BE_render_manager_sdl;

// -------------------------------------------------------------------------------------------------

/**
 * @brief Data layout of a "sdl window" base entity. Use it to configure title, position, dimension and flags on entity creation.
 */
typedef struct BE_window_sdl {
    /** Title of the window used on window creation. */
    const char *title;
    /** Pixel position of the window on creation. */
    size_t x, y;
    /** Pixel position of the window on creation. */
    size_t w, h;
    /** Flags passed to the SDL window creation method on initialization of the entity. See the SDL_WindowFlags enum for values. */
    SDL_WindowFlags flags;

    /** Pointer to a window that will be created on entity initialization. Overriden on initialization. */
    SDL_Window *window;
} BE_window_sdl;

// -------------------------------------------------------------------------------------------------

/**
 * @brief Represents information about an object in a 2D space.
 * Not very useful by itself, but widely used in entities such as BE_body_2D.
 */
typedef struct properties_2D {
    /** Coordinates of the object in the plane. */
    vector2_t position;
    /** Scale of the object. */
    vector2_t scale;
    /** Angle of the object, in radians. */
    f32 angle;
} properties_2D;

/**
 * @brief Data layout of a "2D body" entity. Use it to configure 2D information of objects interacting in the world plane, or change the position of an object through its local position.
 */
typedef struct BE_body_2D BE_body_2D;
typedef struct BE_body_2D {
    /** Parent 2D body this body is pulling its global position from, automatically pulled from the entity's parents. Could be NULL. Overriden on initialization. */
    BE_body_2D *previous;

    /** Local position of the object in respect to its optional previous parent 2D body. */
    properties_2D local;
    /** Global position of the object on the world plane. Overriden on each frame. */
    properties_2D global;
} BE_body_2D;

// -------------------------------------------------------------------------------------------------

/**
 * @brief Data layout of a "2D texture" entity. Use it to configure what texture is displayed, on what index, and where.
 */
typedef struct BE_texture_2D {
    /** Position information of the texture entity. */
    BE_body_2D body;
    /** Draw index to register a drawing operation on initialization. */
    i32 draw_index;

    /** Texture rendered to the screen. */
    SDL_Texture *texture;
} BE_texture_2D;

// -------------------------------------------------------------------------------------------------

typedef enum shape_2D_id {
    SHAPE_2D_CIRCLE,
    SHAPE_2D_RECT,
} shape_2D_id;

typedef struct shape_2D_circle {
    f32 radius;
} shape_2D_circle;

typedef struct shape_2D_rect {
    f32 width;
    f32 height;
} shape_2D_rect;

typedef struct BE_shape_2D {
    BE_body_2D body;
    shape_2D_id kind;

    union { shape_2D_circle as_circle; shape_2D_rect as_rect; };
} BE_shape_2D;

// -------------------------------------------------------------------------------------------------

typedef struct BE_shape_2D_visual {
    BE_shape_2D *visualized;

    SDL_Color color;
    i32 draw_index;
} BE_shape_2D_visual;

// -------------------------------------------------------------------------------------------------

typedef u32 collision_bitmask;

typedef struct BE_collision_manager_2D BE_collision_manager_2D;

typedef struct BE_shape_2D_collider {
    BE_shape_2D *monitored;
    BE_collision_manager_2D *manager;

    collision_bitmask mask_detected_on;
    collision_bitmask mask_can_detect_on;

    // + callback
} BE_shape_2D_collider;

vector2_t BE_shape_2D_collider_support(BE_shape_2D_collider *col, vector2_t direction);

// -------------------------------------------------------------------------------------------------

typedef struct BE_collision_manager_2D {
    RANGE(BE_shape_2D_collider *) *registered_collisions;
} BE_collision_manager_2D;

void BE_collision_manager_2D_register_shape(BE_collision_manager_2D *collision_manager, BE_shape_2D_collider *col);
void BE_collision_manager_2D_unregister_shape(BE_collision_manager_2D *collision_manager, BE_shape_2D_collider *col);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* Entity definition of a SDL context, managing the broader SDL lifetime. */
extern const tarasque_entity_definition BE_DEF_context_sdl;

// -------------------------------------------------------------------------------------------------

/* Entity definition of a SDL event relay, sending back events polled from the SDL library on the event stack. */
extern const tarasque_entity_definition BE_DEF_event_relay_sdl;

// -------------------------------------------------------------------------------------------------

/* Entity definition of a SDL render manager, that sends draw events through the game tree to draw content onto a window. */
extern const tarasque_entity_definition BE_DEF_render_manager_sdl;

// -------------------------------------------------------------------------------------------------

/* Entity definition of a SDL window, that creates a desktop window.*/
extern const tarasque_entity_definition BE_DEF_window_sdl;

// -------------------------------------------------------------------------------------------------

/* Entity definition of an object that is positioned in a 2D space. */
extern const tarasque_entity_definition BE_DEF_body_2D;

// -------------------------------------------------------------------------------------------------

/* Entity definition of an textured object that is positioned in a 2D space. */
extern const tarasque_entity_definition BE_DEF_texture_2D;

// -------------------------------------------------------------------------------------------------

extern const tarasque_entity_definition BE_DEF_shape_2D;

// -------------------------------------------------------------------------------------------------

extern const tarasque_entity_definition BE_DEF_shape_2D_visual;

// -------------------------------------------------------------------------------------------------

extern const tarasque_entity_definition BE_DEF_shape_2D_collider;

// -------------------------------------------------------------------------------------------------

extern const tarasque_entity_definition BE_DEF_collision_manager_2D;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

#endif
