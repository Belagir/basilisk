
#include "base_entity_algorithm_collision_2D.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

typedef struct simplex { vector2_t A; vector2_t B; vector2_t C; } simplex;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/*  */
static vector2_t BE_shape_2D_collider_GJK_support_function(BE_shape_2D_collider *c1, BE_shape_2D_collider *c2, vector2_t direction);

/*  */
static simplex BE_shape_2D_collider_GJK_create_simplex(BE_collision_manager_2D *collision_manager, BE_shape_2D_collider *c1, BE_shape_2D_collider *c2);

/*  */
static bool BE_shape_2D_collider_GJK_simplex_contains_origin(simplex tested);

/*  */
static bool BE_shape_2D_collider_GJK_segment_Voronoi_contains_origin(vector2_t start, vector2_t end, vector2_t control);

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
bool BE_collision_manager_2D_check(BE_collision_manager_2D *collision_manager, BE_shape_2D_collider *c1, BE_shape_2D_collider *c2)
{
    simplex shape = { 0u };
    bool collision = false;

    // 1 - create the optimal simplex
    shape = BE_shape_2D_collider_GJK_create_simplex(collision_manager, c1, c2);

    // 2 - check for collision
    collision = BE_shape_2D_collider_GJK_simplex_contains_origin(shape);

    return collision;
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
    vector2_t tmp = { 0u };
    simplex returned_simplex = { 0u };
    bool simplex_needs_optimisation = false;

    if (!collision_manager || !c1 || !c2) {
        return (simplex) { 0u };
    }

    // choosing a vector perpendicular to the shapes' direction to each other has a good chance to only need one iteration
    direction = vector2_normal_of(vector2_substract(
            BE_body_2D_global(BE_shape_2D_collider_get_body(c2)).position,
            BE_body_2D_global(BE_shape_2D_collider_get_body(c1)).position));
    returned_simplex.A = BE_shape_2D_collider_GJK_support_function(c1, c2, direction);

    // opposite direction
    direction = vector2_negate(direction);
    returned_simplex.B = BE_shape_2D_collider_GJK_support_function(c1, c2, direction);

    // normal vector from AB to AO (O is the origin)
    direction = vector2_normalize(vector2_triple_product(
            vector2_substract(returned_simplex.B, returned_simplex.A),
            vector2_substract(VECTOR2_ORIGIN,     returned_simplex.A),
            vector2_substract(returned_simplex.B, returned_simplex.A)));
    returned_simplex.C = BE_shape_2D_collider_GJK_support_function(c1, c2, direction);

    // step the simplex to guarantee that it contains the origin
    for (size_t i = 0u ; i < 2u ; i++){
        if (BE_shape_2D_collider_GJK_segment_Voronoi_contains_origin(returned_simplex.C, returned_simplex.A, returned_simplex.B)) {
            returned_simplex.B = returned_simplex.C;
            simplex_needs_optimisation = true;
        } else if (BE_shape_2D_collider_GJK_segment_Voronoi_contains_origin(returned_simplex.C, returned_simplex.B, returned_simplex.A)) {
            returned_simplex.A = returned_simplex.C;
            simplex_needs_optimisation = true;
        }

        if (simplex_needs_optimisation) {
            // normal vector from AB to AO (O is the origin)
            direction = vector2_normalize(vector2_triple_product(
                    vector2_substract(returned_simplex.B, returned_simplex.A),
                    vector2_substract(VECTOR2_ORIGIN,     returned_simplex.A),
                    vector2_substract(returned_simplex.B, returned_simplex.A)));
            returned_simplex.C = BE_shape_2D_collider_GJK_support_function(c1, c2, direction);
        }
    }

    return returned_simplex;
}

/**
 * @brief
 *
 * The point at index 2 (C) must be the most recently added point in the simplex.
 *
 * @param collision_manager
 * @param simplex
 * @return
 */
static bool BE_shape_2D_collider_GJK_simplex_contains_origin(simplex tested)
{
    return !(BE_shape_2D_collider_GJK_segment_Voronoi_contains_origin(tested.C, tested.A, tested.B)
            || BE_shape_2D_collider_GJK_segment_Voronoi_contains_origin(tested.C, tested.B, tested.A));
}

/**
 * @brief
 *
 * @param start
 * @param end
 * @param renderer
 * @return
 */
static bool BE_shape_2D_collider_GJK_segment_Voronoi_contains_origin(vector2_t start, vector2_t end, vector2_t control)
{
    vector2_t normal = VECTOR2_ZERO;
    f32 dot_to_origin = 0.f;
    vector2_t to_origin = VECTOR2_ZERO;

    to_origin = vector2_normalize(vector2_substract(VECTOR2_ORIGIN, start));
    normal = vector2_normalize(vector2_triple_product(
            vector2_substract(control, start),
            vector2_substract(end, start),
            vector2_substract(end, start)));
    dot_to_origin = vector2_dot_product(to_origin, normal);

    return (dot_to_origin > 0.f);
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

