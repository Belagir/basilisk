
#include <base_entities/sdl_entities.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/*  */
static void BE_shape_2D_collider_init(tarasque_entity *self_data);

/*  */
static void BE_shape_2D_collider_deinit(tarasque_entity *self_data);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param self_data
 */
static void BE_shape_2D_collider_init(tarasque_entity *self_data)
{
    BE_shape_2D_collider *collider = (BE_shape_2D_collider *) self_data;

    collider->monitored = tarasque_entity_get_parent(collider, NULL, &BE_DEF_shape_2D);

    if (collider->monitored) {
        collider->manager = tarasque_entity_get_parent(collider, NULL, &BE_DEF_collision_manager_2D);
        BE_collision_manager_2D_register_shape(collider->manager, collider);
    }
}

/**
 * @brief
 *
 * @param self_data
 */
static void BE_shape_2D_collider_deinit(tarasque_entity *self_data)
{
    BE_shape_2D_collider *collider = (BE_shape_2D_collider *) self_data;

    BE_collision_manager_2D_unregister_shape(collider->manager, collider);
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param col
 * @param direction
 * @return
 */
vector2_t BE_shape_2D_collider_support(BE_shape_2D_collider *col, vector2_t direction)
{
    if (!col || !col->monitored) {
        return (vector2_t) { NAN, NAN };
    }

    switch (col->monitored->kind) {
        case SHAPE_2D_CIRCLE:
            return vector2_add(col->monitored->body.global.position, vector2_scale(col->monitored->as_circle.radius, direction));
            break;
        case SHAPE_2D_RECT:
            // TODO : rectangle support function
            break;
    }

    return (vector2_t) { NAN, NAN };
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 */
const tarasque_entity_definition BE_DEF_shape_2D_collider = {
        .data_size = sizeof(BE_shape_2D_collider),

        .on_init = &BE_shape_2D_collider_init, // register to a parent collision manager
        .on_deinit = &BE_shape_2D_collider_deinit, // unregister from the collision manager
};
