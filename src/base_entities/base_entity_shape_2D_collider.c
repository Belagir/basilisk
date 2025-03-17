/**
 * @file base_entity_shape_2D_collider.c
 * @author gabriel ()
 * @brief Implementation file for a shape collider base entity.
 * @version 0.1
 * @date 2024-05-03
 *
 * @copyright Copyright (c) 2024
 *
 */
#include <base_entities/sdl_entities.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Private layout of a shape collider base entity.
 *
 */
typedef struct BE_shape_2D_collider {
    /** Pointer to a parent shape entity to provide a collision box. */
    basilisk_entity *monitored;
    /** Pointer to a parent collision manager entity to register to. */
    basilisk_entity *manager;

    /** Callback data describing the behavior on a detected collision. */
    shape_2D_collider_callback_info callback_info;
} BE_shape_2D_collider;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* Initialisation callback for a BE_shape_2D_collider entity. */
static void BE_shape_2D_collider_init(basilisk_entity *self_data);

/* De-initialisation callback for a BE_shape_2D_collider entity. */
static void BE_shape_2D_collider_deinit(basilisk_entity *self_data);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Initialises a BE_shape_2D_collider entity by finding a shape parent and a collision manager entities to hook to.
 *
 * @param[inout] self_data pointer to a BE_shape_2D_collider object.
 */
static void BE_shape_2D_collider_init(basilisk_entity *self_data)
{
    BE_shape_2D_collider *collider = (BE_shape_2D_collider *) self_data;

    collider->monitored = basilisk_entity_get_parent(collider, NULL, &ENTITY_DEF_SHAPE_2D);

    if (collider->monitored) {
        collider->manager = basilisk_entity_get_parent(collider, NULL, &ENTITY_DEF_COLLISION_MANAGER_2D);
        collision_manager_2D_register_shape(collider->manager, collider);
    }
}

/**
 * @brief De-initialises a BE_shape_2D_collider entity by unregesistering its collision callback from its eventual collision manager parent.
 *
 * @param[inout] self_data pointer to a BE_shape_2D_collider object.
 */
static void BE_shape_2D_collider_deinit(basilisk_entity *self_data)
{
    BE_shape_2D_collider *collider = (BE_shape_2D_collider *) self_data;

    collision_manager_2D_unregister_shape(collider->manager, collider);
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Returns the support point (the point belonging to the shape that is furthest in a direction) of the extended parent shape in a specified direction.
 * This function is vital for the BE_collision_manager_2D base entity. It provides all the information it needs to compute the collision profile of a shape.
 *
 * @param[in] col collider entity to examine
 * @param[in] direction direction of the support point
 * @return vector2_t
 */
vector2_t shape_2D_collider_support(const basilisk_entity *col_entity, vector2_t direction)
{
    properties_2D properties = { 0u };

    if (!basilisk_entity_is(col_entity, ENTITY_DEF_SHAPE_2D_COLLIDER)) {
        return (vector2_t) { NAN, NAN };
    }

    const BE_shape_2D_collider *col = (const BE_shape_2D_collider *) col_entity;

    if (!col->monitored) {
        return (vector2_t) { NAN, NAN };
    }

    // TODO : test with scales changed

    properties = body_2D_global(shape_2D_get_body(col->monitored));

    switch (shape_2D_what(col->monitored)) {
        case SHAPE_2D_CIRCLE:
            return vector2_add(properties.position, vector2_scale(shape_2D_as_circle(col->monitored)->radius, direction));
            break;
        case SHAPE_2D_RECT:
            if ((direction.x <= 0.f) && (direction.y <= 0.f)) {           // upper left corner
                return properties.position;
            } else if ((direction.x > 0.f) && (direction.y <= 0.f)) {    // upper right corner
                return vector2_add(properties.position, (vector2_t) { .x = shape_2D_as_rect(col->monitored)->width });
            } else if ((direction.x > 0.f) && (direction.y > 0.f)) {    // lower right corner
                return vector2_add(properties.position, (vector2_t) { .x = shape_2D_as_rect(col->monitored)->width, .y = shape_2D_as_rect(col->monitored)->height });
            } else {                                                    // lower left corner
                return vector2_add(properties.position, (vector2_t) { .y = shape_2D_as_rect(col->monitored)->height });
            }
            break;
    }

    return (vector2_t) { NAN, NAN };
}

/**
 * @brief Returns the body 2D entity the collider's parent shape might have as a parent.
 *
 * @param[in] col collider entity to examine
 * @return BE_body_2D*
 */
basilisk_entity *shape_2D_collider_get_body(const basilisk_entity *col_entity)
{
    if (!basilisk_entity_is(col_entity, ENTITY_DEF_SHAPE_2D_COLLIDER)) {
        return nullptr;
    }

    const BE_shape_2D_collider *col = (const BE_shape_2D_collider *) col_entity;

    return shape_2D_get_body(col->monitored);
}

/**
 * @brief Modifies the callback information of a collider entity. The new callback will be executed on a collision with this collider.
 *
 * @param[inout] col collider entity to modify
 * @param[in] callback new callback information
 */
void shape_2D_collider_set_callback(basilisk_entity *col_entity, shape_2D_collider_callback_info callback)
{
    if (!basilisk_entity_is(col_entity, ENTITY_DEF_SHAPE_2D_COLLIDER)) {
        return;
    }

    BE_shape_2D_collider *col = (BE_shape_2D_collider *) col_entity;

    if (!col) {
        return;
    }

    col->callback_info = callback;
}

/**
 * @brief Executes the callback stored in a collider entity, if it exists.
 *
 * @param[inout] hit collider that detected the hit
 * @param[in] other other collider the first collider detected
 * @param[in] collision_info collision information passed to the collision callback
 */
void shape_2D_collider_exec_callback(basilisk_entity *hit_entity, basilisk_entity *other_entity, collision_2D_info collision_info)
{
    if (!basilisk_entity_is(hit_entity, ENTITY_DEF_SHAPE_2D_COLLIDER) || !basilisk_entity_is(other_entity, ENTITY_DEF_SHAPE_2D_COLLIDER)) {
        return;
    }

    BE_shape_2D_collider *hit = (BE_shape_2D_collider *) hit_entity;

    if (!hit->callback_info.callback) {
        return;
    }

    hit->callback_info.callback(hit->callback_info.subject, hit, other_entity, collision_info);
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Defines the properties of a BE_shape_2D_collider entity.
 *
 * A shape visual is meant to be a child of both a BE_shape_2D entity and a BE_collision_manager_2D, and if it is not, the entity will have no effect.
 *
 * The BE_shape_2D_visual entity provides a way to extend the functionalities of a BE_shape_2D entity so it can detect collision with other shapes, using the parent BE_shape_2D as the collision shape.
 *
 * @see BE_shape_2D_collider
 *
 */
const basilisk_entity_definition ENTITY_DEF_SHAPE_2D_COLLIDER = {
        .data_size = sizeof(BE_shape_2D_collider),

        .on_init = &BE_shape_2D_collider_init, // register to a parent collision manager
        .on_deinit = &BE_shape_2D_collider_deinit, // unregister from the collision manager
};

basilisk_specific_entity shape_2D_collider(void)
{
    return (basilisk_specific_entity) {
            .entity_def = ENTITY_DEF_SHAPE_2D_COLLIDER,
            .data = nullptr,
    };
}
