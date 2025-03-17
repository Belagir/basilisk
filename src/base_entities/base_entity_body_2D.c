/**
 * @file base_entity_body_2D.c
 * @author gabriel ()
 * @brief Implementation file of the BE_body_2D entity.
 *
 * @version 0.1
 * @date 2024-04-22
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <ustd/math.h>

#include <base_entities/sdl_entities.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Private data layout of a "2D body" entity. Use it to configure 2D information of objects positioned in the world plane.
 *
 * @see BE_STATIC_body_2D, ENTITY_DEF_BODY_2D
 */
struct BE_body_2D {
    /** Parent 2D body this body is pulling its global position from, automatically pulled from the entity's parents. Could be NULL. Overriden on initialisation. */
    struct BE_body_2D *previous;

    /** Local position of the object in respect to its optional previous parent 2D body. */
    properties_2D local;
    /** Global position of the object on the world plane. Overriden on each frame. */
    properties_2D global;
};

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* BE_body_2D initialisation callback. */
static void BE_body_2D_init(basilisk_entity *self_data);

/* BE_body_2D time step callback.*/
static void BE_body_2D_on_frame(basilisk_entity *self_data, float elapsed_ms);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* Updates the global properties of a BE_body_2D object from its local properties. */
static void BE_body_2D_update(struct BE_body_2D *body);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Initialisation callback for a BE_body_2D entity.
 * Fetches an eventual BE_body_2D parent to sync its own position relative to the global position of this parent.
 *
 * @param[inout] self_data pointer to a BE_body_2D object
 */
static void BE_body_2D_init(basilisk_entity *self_data)
{
    if (!self_data) {
        return;
    }

    struct BE_body_2D *self_body = (struct BE_body_2D *) self_data;

    self_body->previous = basilisk_entity_get_parent(self_data, NULL, &ENTITY_DEF_BODY_2D);
    BE_body_2D_update(self_body);
}

/**
 * @brief Frame callback for a BE_body_2D entity.
 * Steps the body through time, updating its global position if it can.
 *
 * @param[inout] self_data pointer to a BE_body_2D object
 * @param[in] elapsed_ms number of milliseconds that passed since the last frame
 */
static void BE_body_2D_on_frame(basilisk_entity *self_data, float elapsed_ms)
{
    (void) elapsed_ms;

    if (!self_data) {
        return;
    }

    struct BE_body_2D *self_body = (struct BE_body_2D *) self_data;
    BE_body_2D_update(self_body);
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Synchronise the global position of a BE_body_2D relative to the global position of its parent (if it exists) using its own local position.
 *
 * @param[inout] body target body to update
 */
static void BE_body_2D_update(struct BE_body_2D *body)
{
    if (!body) {
        return;
    }

    if (body->previous) {
        body->global.scale    = vector2_members_product(body->previous->global.scale, body->local.scale);
        body->global.position = vector2_members_product(body->local.scale, vector2_add(body->previous->global.position, body->local.position));
        body->global.angle    = fmodf(body->previous->global.angle + body->local.angle, PI_T_2);
    } else {
        body->global = body->local;
    }
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------


/**
 * @brief Returns the local properties of a BE_body_2D entity. Those properties are relative to an eventual other parent BE_body_2D.
 *
 * @param[in] body BE_body_2D to examine
 * @return properties_2D
 */
properties_2D body_2D_local(const basilisk_entity *body_entity)
{
    if (!body_entity) {
        return (properties_2D) { 0u };
    }

    const struct BE_body_2D *body = (const struct BE_body_2D *) body_entity;

    return body->local;
}

/**
 * @brief Returns the global properties of a BE_body_2D entity.
 *
 * @param[in] body BE_body_2D to examine
 * @return properties_2D
 */
properties_2D body_2D_global(const basilisk_entity *body_entity)
{
    if (!body_entity) {
        return (properties_2D) { 0u };
    }

    const struct BE_body_2D *body = (const struct BE_body_2D *) body_entity;

    return body->global;
}

/**
 * @brief Overrides the local properties of a BE_body_2D entity.
 * This will also update the global properties of the BE_body_2D entity.
 *
 * @param[inout] body_2D BE_body_2D entity to modify
 * @param[in] new_properties new local properties of the BE_body_2D entity
 */
void body_2D_local_set(basilisk_entity *body_entity, properties_2D new_properties)
{
    if (!body_entity) {
        return;
    }

    struct BE_body_2D *body = (struct BE_body_2D *) body_entity;

    body->local = new_properties;
    BE_body_2D_update(body);
}

/**
 * @brief Offsets the position of a BE_body_2D entity with a 2D vector.
 * This will also update the global properties of the BE_body_2D entity.
 *
 * @param[inout] body_2D BE_body_2D entity to move
 * @param[in] change movement vector
 */
void body_2D_translate(basilisk_entity *body_entity, vector2_t change)
{
    if (!body_entity) {
        return;
    }

    struct BE_body_2D *body = (struct BE_body_2D *) body_entity;

    body->local.position = vector2_add(body->local.position, change);
    BE_body_2D_update(body);
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Defines the entity properties of a BE_body_2D entity.
 *
 * The goal of this entity is to provide a positioning utility that uses the game tree to determine its position relative to other 2D body parents.
 *
 * A 2D body is an entity containing positionning information in a 2D world : axis coordinates, angle and scale (see the `properties_2D` struct for details).
 * This entity separates two positions : a local one (accessed with `BE_body_local()`) and a global one (accessed with `BE_body_global()`). A 2D body will,
 * on initialisation, try to hook to a parent 2D body : this parent's position will be used to create the 2D body entity own global position by adding its
 * local position.
 * If no such parent is found, then the local position of the entity is the same as its global position.
 *
 * However, you can only change the entity's local position with `body_2D_local_set()` and other setter functions.
 *
 * @see BE_body_2D
 *
 */
const basilisk_entity_definition ENTITY_DEF_BODY_2D = {
        .data_size = sizeof(struct BE_body_2D),
        .on_init = BE_body_2D_init,
        .on_frame = &BE_body_2D_on_frame,
};

struct basilisk_specific_entity create_body_2D(properties_2D properties)
{
    static struct BE_body_2D buffer = { 0u };

    buffer = (struct BE_body_2D) { 0u };
    buffer = (struct BE_body_2D) {
            .local = properties,
    };

    return (struct basilisk_specific_entity) {
            .entity_def = ENTITY_DEF_BODY_2D,
            .data = &buffer
    };
}