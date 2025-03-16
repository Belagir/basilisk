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
#ifndef __BASILISK_BASE_ENTITIES_SDL_ENTITIES_H__
#define __BASILISK_BASE_ENTITIES_SDL_ENTITIES_H__

#include <SDL2/SDL.h>
#include "../unstandard/inc/ustd/math2d.h"
#include "basilisk.h"

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
 * @brief Kinds of shapes the base entities can represent and use.
 */
typedef enum shape_2D_id {
    /** Simple circle configured by its radius. */
    SHAPE_2D_CIRCLE,
    /** Simple axis-aligned rectangle configured by its width and length. */
    SHAPE_2D_RECT,
} shape_2D_id;

/**
 * @brief Implementation of a circle shape.
 */
typedef struct shape_2D_circle {
    /** Radius, in pixels, of the circle. */
    f32 radius;
} shape_2D_circle;

/**
 * @brief Implementation of a rectangle shape.
 */
typedef struct shape_2D_rect {
    /** X-axis length, in pixels, of the rectangle. */
    f32 width;
    /** Y-axis length, in pixels, of the rectangle. */
    f32 height;
} shape_2D_rect;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// ---- SDL CONTEXT ENTITY ----
// Manage the SDL library initialisation and exit. This is not a necessary parent for other SDL-related entities, but provides a clean lifetime to SDL modules.

/* Entity definition of a SDL context, managing the broader SDL lifetime. */
extern const basilisk_entity_definition ENTITY_DEF_CONTEXT_SDL;

/* Returns statically-allocated SDL Context entity data, used to give the engine data to copy for instanciation. */
struct basilisk_specific_entity create_context_sdl(void);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// ---- SDL EVENT RELAY ENTITY ----
// Sends back SDL events (polled `SDL_Event` objects) through the engine.


/* Entity definition of a SDL event relay, sending back events polled from the SDL library on the event stack. */
extern const basilisk_entity_definition ENTITY_DEF_EVENT_RELAY_SDL;

/* Returns statically-allocated SDL event relay entity data, used to give the engine data to copy for instanciation. */
struct basilisk_specific_entity create_event_relay_sdl(void);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// ---- SDL WINDOW ENTITY ----
// Wraps around a SDL window.

/* Entity definition of a SDL window, that creates a desktop window.*/
extern const basilisk_entity_definition ENTITY_DEF_WINDOW_SDL;

/* Returns statically-allocated SDL window entity data, used to give the engine data to copy for instanciation. */
struct basilisk_specific_entity create_window_sdl(const char *title, size_t w, size_t h, size_t x, size_t y, SDL_WindowFlags flags);

// -------------------------------------------------------------------------------------------------

/* Returns the SDL handle to the window managed by this entity. */
SDL_Window *BE_window_sdl_get_window(basilisk_entity *window_entity);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// ---- SDL RENDER MANAGER ENTITY ----
// Orchestrate draw operations on a parent window, providing a "sdl renderer draw" event to other entities.

/**
 * @brief Data layout of a "sdl renderer draw" drawing event companion data sent by the BE_render_manager_sdl_entity_def entity.
 */
typedef struct BE_render_manager_sdl_event_draw {
    /** Renderer that is currently accepting drawing operations. */
    SDL_Renderer *renderer;
    // TODO : add information about the rendering target : pointer to texture
} BE_render_manager_sdl_event_draw;

/* Entity definition of a SDL render manager, that sends draw events through the game tree to draw content onto a window. */
extern const basilisk_entity_definition ENTITY_DEF_RENDER_MANAGER_SDL;

/* Returns statically-allocated SDL render manager entity data, used to give the engine data to copy for instanciation. */
struct basilisk_specific_entity create_render_manager_sdl(SDL_Color clear_color, size_t w, size_t h, SDL_RendererFlags flags);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// ---- 2D BODY ENTITY ----
// Automates positioning of 2D objects, and creates a hierarchy of 2D objects that have a position relative to each other.

/* Opaque type to a body 2D instance. */
typedef struct BE_body_2D BE_body_2D;

// -------------------------------------------------------------------------------------------------

/* Entity definition of an object that is positioned in a 2D space. */
extern const basilisk_entity_definition ENTITY_DEF_BODY_2D;

/* Returns statically-allocated body 2D data, used to give the engine data to copy for instanciation. */
basilisk_entity *BE_STATIC_body_2D(properties_2D properties);

// -------------------------------------------------------------------------------------------------

/* Returns the local (relative to its first 2D body parent) properies of a 2D body. */
properties_2D BE_body_2D_local(const BE_body_2D *body);
/* Returns the global properties of a 2D body. */
properties_2D BE_body_2D_global(const BE_body_2D *body);
/* Overwrites the local properties of a 2D body. */
void BE_body_2D_local_set(BE_body_2D *body, properties_2D new_properties);
/* Linearly translates a 2D body. */
void BE_body_2D_translate(BE_body_2D *body, vector2_t change);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// ---- 2D TEXTURE ENTITY ----
// Provides a simple entity to draw an already loaded SDL texture to the screen.

/* Opaque type to a 2D texture instance. */
typedef struct BE_texture_2D BE_texture_2D;

// -------------------------------------------------------------------------------------------------

/* Entity definition of a textured object that is positioned in a 2D space. */
extern const basilisk_entity_definition ENTITY_DEF_TEXTURE_2D;

/* Returns statically-allocated texture 2D data, used to give the engine data to copy for instanciation. */
basilisk_entity *BE_STATIC_texture_2D(SDL_Texture *texture, i32 draw_index);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// ---- 2D SHAPE ENTITY ----
// Entity representing a basic shape easily extended with a visual or a collision box.

/* Opaque type to a 2D shape instance. */
typedef struct BE_shape_2D BE_shape_2D;

// -------------------------------------------------------------------------------------------------

/* Entity definition for a basic 2D shape. It brings no function by itself, add a BE_shape_2D_visual and/or BE_shape_2D_collider below it to have an effect. */
extern const basilisk_entity_definition ENTITY_DEF_SHAPE_2D;

/* Returns statically-allocated circle shape data, used to give the engine data to copy for instanciation.*/
basilisk_entity *BE_STATIC_shape_2D_circle(shape_2D_circle circle);

/* Returns statically-allocated rectangle shape data, used to give the engine data to copy for instanciation. */
basilisk_entity *BE_STATIC_shape_2D_rectangle(shape_2D_rect rect);

// -------------------------------------------------------------------------------------------------

/* Returns the kind of shape an entity is. */
shape_2D_id BE_shape_2D_what(const BE_shape_2D *shape);

/* Returns a shape's circle information. */
shape_2D_circle *BE_shape_2D_as_circle(BE_shape_2D *shape);

/* Returns a shape's rectangle information. */
shape_2D_rect *BE_shape_2D_as_rect(BE_shape_2D *shape);

/* Returns the body a shape as connected to, if any. */
BE_body_2D *BE_shape_2D_get_body(BE_shape_2D *shape);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// ---- 2D SHAPE VISUAL ENTITY ----
// Entity providing a visual to a parent shape entity.

/* Opaque type to a 2D shape visual instance. */
typedef struct BE_shape_2D_visual BE_shape_2D_visual;

// -------------------------------------------------------------------------------------------------

/* Entity definition for a visual extending a 2D shape. */
extern const basilisk_entity_definition ENTITY_DEF_SHAPE_2D_VISUAL;

/* Returns statically-allocated shape visual data, used to give the engine data to copy for instanciation. */
basilisk_entity *BE_STATIC_shape_2D_visual(SDL_Color color, i32 draw_index);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// ---- 2D SHAPE COLLIDER ENTITY ----
// Entity providing a collision object to a parent shape, automatically associating to a parent collision manager.

/* Opaque type to a collision shape entity instance. */
typedef struct BE_shape_2D_collider BE_shape_2D_collider;

/**
 * @brief Contains basic information about a collision happening between two objects.
 */
typedef struct collision_2D_info {
    /** Direction of the collision. The normal points toward the other object. */
    vector2_t normal;
} collision_2D_info;

/**
 * @brief Callback information needed to resolve a collision.
 */
typedef struct BE_shape_2D_collider_callback_info {
    /** Entity the callback will take as subject when resolving the collision. */
    basilisk_entity *subject;
    /** Pointer to a function to be executed on a collision. */
    void (*callback)(basilisk_entity *entity, BE_shape_2D_collider *hit, BE_shape_2D_collider *other, collision_2D_info collision_info);
} BE_shape_2D_collider_callback_info;

// -------------------------------------------------------------------------------------------------

/* Entity definition for a shape collider entity. */
extern const basilisk_entity_definition ENTITY_DEF_SHAPE_2D_COLLIDER;

/* Returns statically-allocated shape collider data, used to give the engine data to copy for instanciation. */
basilisk_entity *BE_STATIC_shape_2D_collider(void);

// -------------------------------------------------------------------------------------------------

/* Returns the support point of a shape collider in a given direction. This is the furthest point of the shape in this direction. */
vector2_t BE_shape_2D_collider_support(const BE_shape_2D_collider *col, vector2_t direction);

/* Returns the body of the shape the collider found. */
BE_body_2D *BE_shape_2D_collider_get_body(const BE_shape_2D_collider *col);

/* Sets the callback executed on a collision involving a shape collider. */
void BE_shape_2D_collider_set_callback(BE_shape_2D_collider *col, BE_shape_2D_collider_callback_info callback);

/* Executes the collision callback associated to a shape collider. */
void BE_shape_2D_collider_exec_callback(BE_shape_2D_collider *hit, BE_shape_2D_collider *other, collision_2D_info collision_info);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// ---- 2D COLLISION MANAGER ENTITY ----
// Collision-detection wrapper around the GJK algorithm shape colliders can register to.

/* Entity definition for a collision manager. */
extern const basilisk_entity_definition ENTITY_DEF_COLLISION_MANAGER_2D;

/* Returns statically-allocated collision manager data, used to give the engine data to copy for instanciation. */
struct basilisk_specific_entity create_collision_manager_2D(void);

// -------------------------------------------------------------------------------------------------

/* Adds a shape collider in the manager so it can detect collisions between it and others. */
void BE_collision_manager_2D_register_shape(basilisk_entity *collision_manager, BE_shape_2D_collider *col);

/* Removes a shape collider from the manager. */
void BE_collision_manager_2D_unregister_shape(basilisk_entity *collision_manager, BE_shape_2D_collider *col);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

#endif
