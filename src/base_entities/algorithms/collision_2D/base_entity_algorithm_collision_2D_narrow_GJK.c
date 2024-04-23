/**
 * @file base_entity_algorithm_collision_2D_narrow_GJK.c
 * @author gabriel ()
 * @brief Implementation file of the GJK algorithm in 2D.
 *
 * The algorithm's goal is to create a simplex (in our case, a triangle) inside the Minkowski substraction of two shapes
 * that might encompass the origin. If it does, then the shapes are colliding.
 *
 * Okay, but what is the Minkowski substraction ?
 * > The Minkowski substraction is a "meta-shape" created by substracting *every* points of a shape from another's.
 * > Since substracting a position from another gives their relative position, and if this relative position is (0, 0),
 * > then we can figure that two points are coinciding. So, if the Minkowski substraction has the point (0, 0) inside,
 * > then the two shapes are in contact (because one (or more) point belonging to one shape substracted to another's
 * > yielded this (0, 0) coordinate).
 *
 * But calculating ALL points of the Minkowski shape sounds kinda bad...
 * > Yes, this is why we only calculate a simplex contained in our target Minkowski substraction. We ask our two shapes their
 * > support points in an arbitrary direction, and substract those two support points : this gives a point on the edge of
 * > our Minkowski substraction. We take such a second point in the opposite direction, and then a third that is perpendicular
 * > to the segment we created, in direction to (0, 0). If this triangle we created contains (0, 0), we can deduce (0, 0)
 * > is part of our Minkowski substraction and that the shapes are colliding. We might compute more points to ensure our simplex
 * > has 100% chances to encompass (0, 0) if it can.
 *
 * Wait, what is as support point ?
 * > Given a direction and a shape, a support point is a point belonging to the shape that is the furthest away from the center of the shape.
 * > This way, we can detect collisions between two arbitrary shapes provided the two of them define such a function.
 *
 *
 * @version 0.1
 * @date 2024-04-22
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "base_entity_algorithm_collision_2D.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief 2D simplex definition. Just three points.
 */
typedef struct simplex { vector2_t A; vector2_t B; vector2_t C; } simplex;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* Provides the support point in some direction of the Minkowski substraction of two shapes. */
static vector2_t BE_shape_2D_collider_GJK_support_function(BE_shape_2D_collider *c1, BE_shape_2D_collider *c2, vector2_t direction);

/* Creates a simplex in the Minkowski substraction  of two shapes trying to encompass the origin.  */
static simplex BE_shape_2D_collider_GJK_create_simplex(BE_shape_2D_collider *c1, BE_shape_2D_collider *c2);

/* Checks that a simplex contains the origin. */
static bool BE_shape_2D_collider_GJK_simplex_contains_origin(simplex tested, collision_2D_info *collision_info);

/* Checks that a Voronoï region described by a segment and a point on the opposite side of the region contains the origin. */
static bool BE_shape_2D_collider_GJK_segment_Voronoi_contains_origin(vector2_t start, vector2_t end, vector2_t control);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Checks that two convex shapes are colliding, using the GJK algorithm.
 *
 * @param[in] c1 first shape collider, must have a parent shape
 * @param[in] c2 second shape collider, must have a parent shape
 * @return true if the shapes are colliding, false otherwise.
 */
bool BE_collision_manager_2D_GJK_check(BE_shape_2D_collider *c1, BE_shape_2D_collider *c2, collision_2D_info *collision_info)
{
    return BE_shape_2D_collider_GJK_simplex_contains_origin(BE_shape_2D_collider_GJK_create_simplex(c1, c2), collision_info);
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------


/**
 * @brief Returns a support point in the Minkowski substraction of two shapes.
 *
 * @param[in] c1 first shape collider, must have a parent shape
 * @param[in] c2 second shape collider, must have a parent shape
 * @param[in] direction normalized vector describing a direction
 * @return vector2_t
 */
static vector2_t BE_shape_2D_collider_GJK_support_function(BE_shape_2D_collider *c1, BE_shape_2D_collider *c2, vector2_t direction)
{
    return vector2_substract(BE_shape_2D_collider_support(c1, direction), BE_shape_2D_collider_support(c2, vector2_negate(direction)));
}

/**
 * @brief Creates the optimal simplex of the Minkowski substraction of the shape c1 and the negation of the shape c2.
 * This simplex is guaranteed to contain the origin of the space this Minkowski substraction lives into.
 *
 * @param[in] c1 first shape collider, must have a parent shape
 * @param[in] c2 second shape collider, must have a parent shape
 * @return simplex
 */
static simplex BE_shape_2D_collider_GJK_create_simplex(BE_shape_2D_collider *c1, BE_shape_2D_collider *c2)
{
    vector2_t direction = VECTOR2_ZERO;
    vector2_t tmp = { 0u };
    simplex returned_simplex = { 0u };
    bool simplex_needs_optimisation = false;

    if (!c1 || !c2) {
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
 * @brief Checks if the origin lies inside the simplex.
 * * The point C of the simplex must be the most recently added point.
 *
 * @param[in] collision_manager
 * @param[in] simplex
 * @return bool
 */
static bool BE_shape_2D_collider_GJK_simplex_contains_origin(simplex tested, collision_2D_info *collision_info)
{
    bool colliding = !(BE_shape_2D_collider_GJK_segment_Voronoi_contains_origin(tested.C, tested.A, tested.B)
            || BE_shape_2D_collider_GJK_segment_Voronoi_contains_origin(tested.C, tested.B, tested.A));

    if (colliding && collision_info) {
        *collision_info = (collision_2D_info) {
                .normal = vector2_normalize(vector2_negate(tested.C)),
        };
    }

    return colliding;
}

/**
 * @brief Checks if some region in space contains the origin.
 * The region begins from by a segment (from `start` to `end`) and expands in the direction opposite to the `control` point.
 *
 * @param[in] start starting point of the segment
 * @param[in] end ending point of the segment
 * @param[in] control point opposite of the region
 * @return bool
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

