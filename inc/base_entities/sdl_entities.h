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
 * @brief
 *
 */
typedef enum shape_2D_id {
    SHAPE_2D_CIRCLE,
    SHAPE_2D_RECT,
} shape_2D_id;

/**
 * @brief
 *
 */
typedef struct shape_2D_circle {
    f32 radius;
} shape_2D_circle;

/**
 * @brief
 *
 */
typedef struct shape_2D_rect {
    f32 width;
    f32 height;
} shape_2D_rect;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* Entity definition of a SDL context, managing the broader SDL lifetime. */
extern const tarasque_entity_definition BE_DEF_context_sdl;
/* Returns statically-allocated SDL Context entity data, used to give the engine data to copy for instanciation. */
tarasque_entity *BE_STATIC_context_sdl(void);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/*  */
typedef struct BE_event_relay_sdl BE_event_relay_sdl;

/* Entity definition of a SDL event relay, sending back events polled from the SDL library on the event stack. */
extern const tarasque_entity_definition BE_DEF_event_relay_sdl;

/* Returns statically-allocated SDL event relay entity data, used to give the engine data to copy for instanciation. */
tarasque_entity *BE_STATIC_event_relay_sdl(void);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Data layout of a "sdl renderer draw" drawing event companion data sent by the BE_render_manager_sdl_entity_def entity.
 */
typedef struct BE_render_manager_sdl_event_draw {
    /** Renderer that is currently accepting drawing operations. */
    SDL_Renderer *renderer;
} BE_render_manager_sdl_event_draw;

/*  */
typedef struct BE_render_manager_sdl BE_render_manager_sdl;

/* Entity definition of a SDL render manager, that sends draw events through the game tree to draw content onto a window. */
extern const tarasque_entity_definition BE_DEF_render_manager_sdl;

/* Returns statically-allocated SDL render manager entity data, used to give the engine data to copy for instanciation. */
tarasque_entity *BE_STATIC_render_manager_sdl(SDL_Color clear_color, SDL_RendererFlags flags);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/*  */
typedef struct BE_window_sdl BE_window_sdl;

/* Entity definition of a SDL window, that creates a desktop window.*/
extern const tarasque_entity_definition BE_DEF_window_sdl;

/* Returns statically-allocated SDL window entity data, used to give the engine data to copy for instanciation. */
tarasque_entity *BE_STATIC_window_sdl(const char *title, size_t w, size_t h, size_t x, size_t y, SDL_WindowFlags flags);
/*  */
SDL_Window *BE_window_sdl_get_window(BE_window_sdl *window_entity);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 */
typedef enum BE_body_2D_space {
    BODY_2D_SPACE_LOCAL,
    BODY_2D_SPACE_GLOBAL,
} BE_body_2D_space;

/* */
typedef struct BE_body_2D BE_body_2D;

/* Entity definition of an object that is positioned in a 2D space. */
extern const tarasque_entity_definition BE_DEF_body_2D;

/* Returns statically-allocated body 2D data, used to give the engine data to copy for instanciation. */
tarasque_entity *BE_STATIC_body_2D(properties_2D properties);
/* */
properties_2D BE_body_2D_get(BE_body_2D *body, BE_body_2D_space how);
/* */
void BE_body_2D_set(BE_body_2D *body, properties_2D new_properties);
/* */
void BE_body_2D_translate(BE_body_2D *body, vector2_t change);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/*  */
typedef struct BE_texture_2D BE_texture_2D;

/* Entity definition of a textured object that is positioned in a 2D space. */
extern const tarasque_entity_definition BE_DEF_texture_2D;

tarasque_entity *BE_STATIC_texture_2D(SDL_Texture *texture, i32 draw_index);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* */
typedef struct BE_shape_2D BE_shape_2D;

/* */
extern const tarasque_entity_definition BE_DEF_shape_2D;
/* */
tarasque_entity *BE_STATIC_shape_2D_circle(shape_2D_circle circle);
/* */
tarasque_entity *BE_STATIC_shape_2D_rectangle(shape_2D_rect rect);

/* */
shape_2D_id BE_shape_2D_what(BE_shape_2D *shape);
/* */
shape_2D_circle *BE_shape_2D_as_circle(BE_shape_2D *shape);
/* */
shape_2D_rect *BE_shape_2D_as_rect(BE_shape_2D *shape);
/* */
BE_body_2D *BE_shape_2D_get_body(BE_shape_2D *shape);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* */
typedef struct BE_shape_2D_visual BE_shape_2D_visual;

/* */
extern const tarasque_entity_definition BE_DEF_shape_2D_visual;

/* */
tarasque_entity *BE_STATIC_shape_2D_visual(SDL_Color color, i32 draw_index);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* */
typedef u32 collision_bitmask;

/* */
typedef struct BE_shape_2D_collider BE_shape_2D_collider;

/* */
extern const tarasque_entity_definition BE_DEF_shape_2D_collider;

/* */
tarasque_entity *BE_STATIC_shape_2D_collider(collision_bitmask mask_detected_on, collision_bitmask mask_can_detect_on);
/* */
vector2_t BE_shape_2D_collider_support(BE_shape_2D_collider *col, vector2_t direction);
/* */
BE_body_2D *BE_shape_2D_collider_get_body(const BE_shape_2D_collider *col);


// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* */
typedef struct BE_collision_manager_2D BE_collision_manager_2D;

/* */
extern const tarasque_entity_definition BE_DEF_collision_manager_2D;

/* */
void BE_collision_manager_2D_register_shape(BE_collision_manager_2D *collision_manager, BE_shape_2D_collider *col);
/* */
void BE_collision_manager_2D_unregister_shape(BE_collision_manager_2D *collision_manager, BE_shape_2D_collider *col);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

#endif
