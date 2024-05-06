/**
 * @file base_entity_collision_manager_2D.c
 * @author gabriel ()
 * @brief Implementation file for the collision manager entity.
 *
 * @version 0.1
 * @date 2024-04-23
 *
 * @copyright Copyright (c) 2024
 *
 */
#include <ustd/sorting.h>
#include <ustd/range.h>

#include <base_entities/sdl_entities.h>

#include "algorithms/collision_2D/base_entity_algorithm_collision_2D.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Private data definition of a collision manager entity.
 *
 * @see BE_DEF_collision_manager_2D, BE_STATIC_collision_manager_2D, BE_shape_2D_collider
 */
typedef struct BE_collision_manager_2D {
    RANGE(BE_shape_2D_collider *) *registered_collisions;
} BE_collision_manager_2D;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* Initialisation callback for the BE_collision_manager_2D entity. */
static void BE_collision_manager_2D_init(tarasque_entity *self_data);

/* Deinitialisation callback for the BE_collision_manager_2D entity. */
static void BE_collision_manager_2D_deinit(tarasque_entity *self_data);

/* Frame callback for the for the BE_collision_manager_2D entity. */
static void BE_collision_manager_2D_frame(tarasque_entity *self_data, float elapsed_time);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* Returns the inverse of some collision info. */
static collision_2D_info collision_2D_info_reflect(collision_2D_info info);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Adds a shape collision entity to the manager. Each frame, the manager will check for collisions between this collider and other colliders registered to it.
 *
 * @param[inout] collision_manager target collision manager
 * @param[in] col added collider
 */
void BE_collision_manager_2D_register_shape(BE_collision_manager_2D *collision_manager, BE_shape_2D_collider *col)
{
    if (!collision_manager || !col) {
        return;
    }

    collision_manager->registered_collisions = range_ensure_capacity(make_system_allocator(), RANGE_TO_ANY(collision_manager->registered_collisions), 1);
    sorted_range_insert_in(RANGE_TO_ANY(collision_manager->registered_collisions), &raw_pointer_compare, &col);
}

/**
 * @brief Removes a shape collider from the list of colliders to monitor.
 *
 * @param[inout] collision_manager target collision manager
 * @param[in] col removed collider
 */
void BE_collision_manager_2D_unregister_shape(BE_collision_manager_2D *collision_manager, BE_shape_2D_collider *col)
{
    if (!collision_manager || !col) {
        return;
    }

    sorted_range_remove_from(RANGE_TO_ANY(collision_manager->registered_collisions), &raw_pointer_compare, &col);
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Initialises the internal data of the entity.
 *
 * @param[in] self_data pointer to a BE_collision_manager_2D object.
 */
static void BE_collision_manager_2D_init(tarasque_entity *self_data)
{
    BE_collision_manager_2D *col_manager = (BE_collision_manager_2D *) self_data;

    col_manager->registered_collisions = range_create_dynamic(make_system_allocator(), sizeof(*col_manager->registered_collisions->data), 8u);
}

/**
 * @brief Deinitialise internals and releases resources taken by the entity.
 *
 * @param[in] self_data pointer to a BE_collision_manager_2D object.
 */
static void BE_collision_manager_2D_deinit(tarasque_entity *self_data)
{
    BE_collision_manager_2D *col_manager = (BE_collision_manager_2D *) self_data;

    range_destroy_dynamic(make_system_allocator(), &RANGE_TO_ANY(col_manager->registered_collisions));
}

/**
 * @brief Checks for collision between all colliders.
 *
 * @param[in] self_data pointer to a BE_collision_manager_2D object.
 * @param[in] elapsed_time milliseconds elapsed since the last frame.
 */
static void BE_collision_manager_2D_frame(tarasque_entity *self_data, float elapsed_time)
{
    BE_collision_manager_2D *col_manager = (BE_collision_manager_2D *) self_data;
    BE_shape_2D_collider *shape_1 = NULL;
    BE_shape_2D_collider *shape_2 = NULL;
    collision_2D_info collision_info = { 0u };

    for (size_t i = 0u ; i < col_manager->registered_collisions->length ; i++) {
        for (size_t j = i + 1u ; j < col_manager->registered_collisions->length ; j++) {
            shape_1 = col_manager->registered_collisions->data[i];
            shape_2 = col_manager->registered_collisions->data[j];
            if (BE_collision_manager_2D_GJK_check(shape_1, shape_2, &collision_info)) {
                BE_shape_2D_collider_exec_callback(shape_1, shape_2, collision_info);
                BE_shape_2D_collider_exec_callback(shape_2, shape_1, collision_2D_info_reflect(collision_info));
            }
        }
    }
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Inverts some collision info that represents a hit from an object 1 to another object 2 so it represents a hit from object 2  to object 1.
 *
 * @param[in] info
 * @return collision_2D_info
 */
static collision_2D_info collision_2D_info_reflect(collision_2D_info info)
{
    return (collision_2D_info) {
            .normal = vector2_negate(info.normal),
    };
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Returns a NULL object, because the BE_collision_manager_2D entity does not need a memory object to be initialised.
 * This function is provided for coherence with other entities and to future proof against possible extentions to this entity.
 *
 * @see BE_DEF_collision_manager_2D, BE_collision_manager_2D, BE_shape_2D_collider
 *
 * @return tarasque_entity *
 */
tarasque_entity *BE_STATIC_collision_manager_2D(void)
{
    return NULL;
}

/**
 * @brief Defines the properties of a BE_collision_manager_2D entity.
 *
 * This entity provides a way to listen for collision between children entities.
 * The collision engine is not finished and supports circles and axis-aligned rectangles, and contains bugs.
 *
 * @see BE_collision_manager_2D, BE_STATIC_collision_manager_2D, BE_shape_2D_collider
 *
 */
const tarasque_entity_definition BE_DEF_collision_manager_2D = {
        .data_size = sizeof(BE_collision_manager_2D),

        .on_init = &BE_collision_manager_2D_init,
        .on_frame = &BE_collision_manager_2D_frame,
        .on_deinit = &BE_collision_manager_2D_deinit,
};