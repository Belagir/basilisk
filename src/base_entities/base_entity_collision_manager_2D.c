
#include <ustd/sorting.h>
#include <ustd/range.h>

#include <base_entities/sdl_entities.h>

#include "algorithms/collision_2D/base_entity_algorithm_collision_2D.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 */
typedef struct BE_collision_manager_2D {
    RANGE(BE_shape_2D_collider *) *registered_collisions;
} BE_collision_manager_2D;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/*  */
static void BE_collision_manager_2D_init(tarasque_entity *self_data);

/*  */
static void BE_collision_manager_2D_deinit(tarasque_entity *self_data);

/*  */
static void BE_collision_manager_2D_frame(tarasque_entity *self_data, float elapsed_time);

/* temp */
static void BE_collision_manager_2D_on_draw(tarasque_entity *self_data, void *event_data);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param collision_manager
 * @param col
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
 * @brief
 *
 * @param collision_manager
 * @param col
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
 * @brief
 *
 * @param self_data
 */
static void BE_collision_manager_2D_init(tarasque_entity *self_data)
{
    BE_collision_manager_2D *col_manager = (BE_collision_manager_2D *) self_data;

    col_manager->registered_collisions = range_create_dynamic(make_system_allocator(), sizeof(*col_manager->registered_collisions->data), 8u);

    tarasque_entity_queue_subscribe_to_event(self_data, "sdl renderer draw", (tarasque_specific_event_subscription) { .callback = &BE_collision_manager_2D_on_draw , .index = 10 });
}

/**
 * @brief
 *
 * @param self_data
 */
static void BE_collision_manager_2D_deinit(tarasque_entity *self_data)
{
    BE_collision_manager_2D *col_manager = (BE_collision_manager_2D *) self_data;

    range_destroy_dynamic(make_system_allocator(), &RANGE_TO_ANY(col_manager->registered_collisions));
}

/**
 * @brief
 *
 * @param self_data
 * @param elapsed_time
 */
// static void BE_collision_manager_2D_frame(tarasque_entity *self_data, float elapsed_time)
// {
//     BE_collision_manager_2D *col_manager = (BE_collision_manager_2D *) self_data;

//     for (size_t i = 0u ; i < col_manager->registered_collisions->length ; i++) {
//         for (size_t j = i + 1u ; j < col_manager->registered_collisions->length ; j++) {
//             if (BE_collision_manager_2D_check(col_manager, col_manager->registered_collisions->data[i], col_manager->registered_collisions->data[j])) {
//                 printf("collision of [%#010x] vs [%#010x]\n", col_manager->registered_collisions->data[i], col_manager->registered_collisions->data[j]);
//             }
//         }
//     }
// }

static void BE_collision_manager_2D_on_draw(tarasque_entity *self_data, void *event_data)
{
    BE_collision_manager_2D *col_manager = (BE_collision_manager_2D *) self_data;
    BE_render_manager_sdl_event_draw *event_draw = (BE_render_manager_sdl_event_draw *) event_data;

    for (size_t i = 0u ; i < col_manager->registered_collisions->length ; i++) {
        for (size_t j = i + 1u ; j < col_manager->registered_collisions->length ; j++) {
            BE_collision_manager_2D_check(col_manager, col_manager->registered_collisions->data[i], col_manager->registered_collisions->data[j], event_draw->renderer);
        }
    }
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 */
const tarasque_entity_definition BE_DEF_collision_manager_2D = {
        .data_size = sizeof(BE_collision_manager_2D),

        .on_init = &BE_collision_manager_2D_init,
        // .on_frame = &BE_collision_manager_2D_frame,
        .on_deinit = &BE_collision_manager_2D_deinit,
};
