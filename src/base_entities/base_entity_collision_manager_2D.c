
#include <ustd/sorting.h>
#include <ustd/range.h>

#include <base_entities/sdl_entities.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

typedef struct simplex {
    vector2_t A;
    vector2_t B;
    vector2_t C;
} simplex;

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
static vector2_t BE_shape_2D_collider_GJK_support_function(BE_shape_2D_collider *c1, BE_shape_2D_collider *c2, vector2_t direction);

/*  */
static simplex BE_shape_2D_collider_GJK_create_simplex(BE_collision_manager_2D *collision_manager, BE_shape_2D_collider *c1, BE_shape_2D_collider *c2);

/*  */
static bool BE_shape_2D_collider_GJK_simplex_contains_origin(BE_collision_manager_2D *collision_manager, simplex tested);

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
                printf("collision of [%#010x] vs [%#010x]\n", col_manager->registered_collisions->data[i], col_manager->registered_collisions->data[j]);
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
 * @param c1
 * @param c2
 * @param direction
 * @return
 */
static vector2_t BE_shape_2D_collider_GJK_support_function(BE_shape_2D_collider *c1, BE_shape_2D_collider *c2, vector2_t direction)
{
    return vector2_substract(BE_shape_2D_collider_support(c1, direction), BE_shape_2D_collider_support(c2, vector2_negate(direction)));
}

/**
 * @brief Creates the optimal simplex of the Minkowski Sum of the shape c1 and the negation of the shape c2.
 * This simplex is guaranteed to contain the origin of the space this Minkowski Addition lives into.
 *
 * @param collision_manager
 * @param c1
 * @param c2
 */
static simplex BE_shape_2D_collider_GJK_create_simplex(BE_collision_manager_2D *collision_manager, BE_shape_2D_collider *c1, BE_shape_2D_collider *c2)
{
    vector2_t direction = VECTOR2_ZERO;
    simplex returned_simplex = { 0u };

    if (!collision_manager || !c1 || !c1->monitored || !c2 || !c2->monitored) {
        return (simplex) { 0u };
    }

    // arbitrary direction that has a good chance to only need one iteration
    direction = vector2_normalize(vector2_substract(c2->monitored->body.global.position, c1->monitored->body.global.position));
    returned_simplex.A = BE_shape_2D_collider_GJK_support_function(c1, c2, direction); // point A

    // opposite direction
    direction = vector2_negate(direction);
    returned_simplex.B = BE_shape_2D_collider_GJK_support_function(c1, c2, direction); // point B

    // normal vector from AB to AO (O is the origin)
    direction = vector2_triple_product(
            vector2_substract(returned_simplex.B, returned_simplex.A),
            vector2_substract(VECTOR2_ORIGIN,     returned_simplex.A),
            vector2_substract(returned_simplex.B, returned_simplex.A));
    returned_simplex.C = BE_shape_2D_collider_GJK_support_function(c1, c2, direction); // point C

    // TODO : step the simplex to guarantee that it contains the origin

    return returned_simplex;
}

/**
 * @brief
 *
 * The point at index 2 must be the most recently added point in the simplex.
 *
 * @param collision_manager
 * @param simplex
 * @return
 */
static bool BE_shape_2D_collider_GJK_simplex_contains_origin(BE_collision_manager_2D *collision_manager, simplex tested)
{
    vector2_t normal_BC = VECTOR2_ZERO;
    vector2_t normal_AC = VECTOR2_ZERO;
    vector2_t CO = VECTOR2_ORIGIN;

    if (!collision_manager) {
        return false;
    }

    CO = vector2_normalize(vector2_substract(VECTOR2_ORIGIN, tested.C));

    normal_BC = vector2_normalize(vector2_triple_product(
            vector2_substract(tested.B, tested.A),
            vector2_substract(tested.C, tested.B),
            vector2_substract(tested.C, tested.B)));

    if (vector2_dot_product(CO, normal_BC) > 0.f) {
        return false;
    }

    normal_AC = vector2_normalize(vector2_triple_product(
            vector2_substract(tested.B, tested.A),
            vector2_substract(tested.C, tested.A),
            vector2_substract(tested.C, tested.A)));

    if (vector2_dot_product(CO, normal_AC) > 0.f) {
        return false;
    }

    return true;
}

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
    simplex shape = { 0u };

    // TODO : collision GJK algorithm

    // 1 - create the optimal simplex
    shape = BE_shape_2D_collider_GJK_create_simplex(collision_manager, c1, c2);

    // 2 - check for collision
    return BE_shape_2D_collider_GJK_simplex_contains_origin(collision_manager, shape);
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
