
#include <ustd/sorting.h>
#include <ustd/range.h>

#include <base_entities/sdl_entities.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

typedef RANGE(vector2_t) range_simplex;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/*  */
static void BE_collision_manager_2D_init(tarasque_entity *self_data);

/*  */
static void BE_collision_manager_2D_deinit(tarasque_entity *self_data);

/*  */
static void BE_collision_manager_2D_frame(tarasque_entity *self_data, float elapsed_time);

// -------------------------------------------------------------------------------------------------

/*  */
static bool BE_collision_manager_2D_check(BE_collision_manager_2D *collision_manager, BE_shape_2D_collider *c1, BE_shape_2D_collider *c2);

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
static void BE_collision_manager_2D_frame(tarasque_entity *self_data, float elapsed_time)
{
    BE_collision_manager_2D *col_manager = (BE_collision_manager_2D *) self_data;

    for (size_t i = 0u ; i < col_manager->registered_collisions->length ; i++) {
        for (size_t j = i + 1u ; j < col_manager->registered_collisions->length ; j++) {
            if (BE_collision_manager_2D_check(col_manager, col_manager->registered_collisions->data[i], col_manager->registered_collisions->data[j])) {
                // TODO : collision callback
            }
        }
    }
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param collision_manager
 * @param c1
 * @param c2
 * @return
 */
static bool BE_collision_manager_2D_check(BE_collision_manager_2D *collision_manager, BE_shape_2D_collider *c1, BE_shape_2D_collider *c2)
{
    bool enclose_origin = false;
    bool can_enclose_origin = true;
    range_simplex *simplex = (range_simplex *) &(RANGE(vector2_t, 3u)) RANGE_CREATE_STATIC(vector2_t, 3u, { });
    vector2_t direction = VECTOR2_X_POSITIVE;
    vector2_t tmp_simplex_point = VECTOR2_ZERO;

    printf("checking [%#010x] (shape id %d) vs [%#010x] (shape id %d)\n", c1, c1->monitored->kind, c2, c2->monitored->kind);

    // TODO : collision GJK algorithm

    // 1 - create the optimal simplex

    // 2 - determine the two voronoid regions

    // 3 - triple product

    // 4 - dot product

    return enclose_origin;
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
        .on_frame = &BE_collision_manager_2D_frame,
        .on_deinit = &BE_collision_manager_2D_deinit,
};
