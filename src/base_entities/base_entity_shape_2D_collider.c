
#include <base_entities/sdl_entities.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 */
typedef struct BE_shape_2D_collider {
    BE_shape_2D *monitored;
    BE_collision_manager_2D *manager;

    BE_shape_2D_collider_callback_info callback_info;
} BE_shape_2D_collider;

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
    properties_2D properties = { 0u };

    if (!col || !col->monitored) {
        return (vector2_t) { NAN, NAN };
    }

    // TODO : test with scales changed

    properties = BE_body_2D_global(BE_shape_2D_get_body(col->monitored));

    switch (BE_shape_2D_what(col->monitored)) {
        case SHAPE_2D_CIRCLE:
            return vector2_add(properties.position, vector2_scale(BE_shape_2D_as_circle(col->monitored)->radius, direction));
            break;
        case SHAPE_2D_RECT:
            if ((direction.x <= 0.f) && (direction.y <= 0.f)) {           // upper left corner
                return properties.position;
            } else if ((direction.x > 0.f) && (direction.y <= 0.f)) {    // upper right corner
                return vector2_add(properties.position, (vector2_t) { .x = BE_shape_2D_as_rect(col->monitored)->width });
            } else if ((direction.x > 0.f) && (direction.y > 0.f)) {    // lower right corner
                return vector2_add(properties.position, (vector2_t) { .x = BE_shape_2D_as_rect(col->monitored)->width, .y = BE_shape_2D_as_rect(col->monitored)->height });
            } else {                                                    // lower left corner
                return vector2_add(properties.position, (vector2_t) { .y = BE_shape_2D_as_rect(col->monitored)->height });
            }
            break;
    }

    return (vector2_t) { NAN, NAN };
}

/**
 * @brief
 *
 * @param col
 * @return BE_body_2D*
 */
BE_body_2D *BE_shape_2D_collider_get_body(const BE_shape_2D_collider *col)
{
    if (!col) {
        return NULL;
    }

    return BE_shape_2D_get_body(col->monitored);
}

/**
 * @brief
 *
 * @param col
 * @param situation
 * @param callback
 */
void BE_shape_2D_collider_set_callback(BE_shape_2D_collider *col, BE_shape_2D_collider_callback_info callback)
{
    if (!col) {
        return;
    }

    col->callback_info = callback;
}

/**
 * @brief
 *
 * @param hit
 * @param other
 */
void BE_shape_2D_collider_exec_callback(BE_shape_2D_collider *hit, BE_shape_2D_collider *other, collision_2D_info collision_info)
{
    if (!hit || !hit->callback_info.callback) {
        return;
    }

    hit->callback_info.callback(hit->callback_info.subject, hit, other, collision_info);
}

/**
 * @brief
 *
 * @param mask_detected_on
 * @param mask_can_detect_on
 * @return
 */
tarasque_entity *BE_STATIC_shape_2D_collider(void)
{
    return NULL;
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
