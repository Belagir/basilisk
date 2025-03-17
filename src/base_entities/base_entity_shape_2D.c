/**
 * @file base_entity_shape_2D.c
 * @author gabriel ()
 * @brief Implementation file for the shape basic entity.
 * @version 0.1
 * @date 2024-04-29
 *
 * @copyright Copyright (c) 2024
 *
 */
#include <base_entities/sdl_entities.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Private data layout of a basic shape entity.
 */
struct BE_shape_2D {
    /** Eventual parent 2D body. */
    basilisk_entity *body;

    /** Specific shape identifier value. */
    shape_2D_id kind;
    /** Specific shape implementation. */
    union { shape_2D_circle as_circle; shape_2D_rect as_rect; };
};

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* Initialises a shape by searching for a BE_body_2D parent. */
static void BE_shape_2D_init(basilisk_entity *self_data);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Initialisation fucntion for a BE_shape_2D.
 * Tries to find a BE_body_2D parent entity to pull its position from.
 *
 * @param[inout] self_data pointer to a BE_shape_2D object.
 */
static void BE_shape_2D_init(basilisk_entity *self_data)
{
    struct BE_shape_2D *shape = (struct BE_shape_2D *) self_data;

    if (!shape) {
        return;
    }

    shape->body = basilisk_entity_get_parent(self_data, NULL, &ENTITY_DEF_BODY_2D);
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Returns the nature of a shape entity.
 *
 * @param[in] shape Shape entity to examine.
 * @return shape_2D_id
 */
shape_2D_id shape_2D_what(const basilisk_entity *shape_entity)
{
    if (!shape_entity) {
        return -1;
    }

    struct BE_shape_2D *shape = (struct BE_shape_2D *) shape_entity;

    return shape->kind;
}

/**
 * @brief Sends the specific data of a shape as if it were a circle.
 * Use in conjunction `shape_2D_what()`.
 *
 * @param[in] shape Shape entity to examine.
 * @return shape_2D_circle *
 */
shape_2D_circle *shape_2D_as_circle(basilisk_entity *shape_entity)
{
    if (!shape_entity) {
        return NULL;
    }

    struct BE_shape_2D *shape = (struct BE_shape_2D *) shape_entity;

    return &(shape->as_circle);
}

/**
 * @brief Sends the specific data of a shape as if it were a rectangle.
 * Use in conjunction `shape_2D_what()`.
 *
 * @param[in] shape Shape entity to examine.
 * @return shape_2D_rect *
 */
shape_2D_rect *shape_2D_as_rect(basilisk_entity *shape_entity)
{
    if (!shape_entity) {
        return NULL;
    }

    struct BE_shape_2D *shape = (struct BE_shape_2D *) shape_entity;

    return &(shape->as_rect);
}

/**
 * @brief Returns the BE_body_2D entity the shape entity might have found.
 *
 * @param[in] shape Shape entity to examine.
 * @return BE_body_2D *
 */
basilisk_entity *shape_2D_get_body(basilisk_entity *shape_entity)
{
    if (!shape_entity) {
        return NULL;
    }

    struct BE_shape_2D *shape = (struct BE_shape_2D *) shape_entity;

    return shape->body;
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Defines the properties of a shape entity.
 *
 * The entity represents an abstract, yet functionless shape. Use this entity as a parent of a BE_shape_2D_collider and / or BE_shape_2D_visual to add your custom behavior to it.
 *
 * @see BE_shape_2D, BE_STATIC_shape_2D, BE_shape_2D_collider, BE_shape_2D_visual
 *
 */
const basilisk_entity_definition ENTITY_DEF_SHAPE_2D = {
        .data_size = sizeof(struct BE_shape_2D),
        .on_init = &BE_shape_2D_init,
};

struct basilisk_specific_entity create_shape_2D_circle(shape_2D_circle circle)
{
    static struct BE_shape_2D buffer = { 0u };

    buffer = (struct BE_shape_2D) { 0u };
    buffer = (struct BE_shape_2D) {
            .kind = SHAPE_2D_CIRCLE,
            .as_circle = circle,
    };

    return (struct basilisk_specific_entity) {
        .entity_def = ENTITY_DEF_SHAPE_2D,
        .data = &buffer,
    };
}

struct basilisk_specific_entity create_shape_2D_rectangle(shape_2D_rect rect)
{
    static struct BE_shape_2D buffer = { 0u };

    buffer = (struct BE_shape_2D) { 0u };
    buffer = (struct BE_shape_2D) {
            .kind = SHAPE_2D_RECT,
            .as_rect = rect,
    };

    return (struct basilisk_specific_entity) {
        .entity_def = ENTITY_DEF_SHAPE_2D,
        .data = &buffer,
    };
}

