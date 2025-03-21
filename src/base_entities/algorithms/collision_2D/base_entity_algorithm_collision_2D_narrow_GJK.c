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

#define GJK_ITERATION_LIMIT (2)

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
static vector2_t BE_shape_2D_collider_GJK_support_function(basilisk_entity *c1, basilisk_entity *c2, vector2_t direction);

/* Creates a simplex in the Minkowski substraction  of two shapes trying to encompass the origin.  */
static simplex BE_shape_2D_collider_GJK_create_simplex(basilisk_entity *c1, basilisk_entity *c2);

/* Checks that a simplex contains the origin. */
static bool BE_shape_2D_collider_GJK_simplex_contains_origin(simplex tested, collision_2D_info *collision_info);

static vector2_t BE_shape_2D_collider_segment_normal_opposite(vector2_t start, vector2_t end, vector2_t control);

static void BE_collision_manager_2D_GJK_draw_simplex(simplex s, SDL_Renderer *renderer);

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
bool BE_collision_manager_2D_GJK_check(basilisk_entity *c1, basilisk_entity *c2, collision_2D_info *collision_info)
{
    simplex s = BE_shape_2D_collider_GJK_create_simplex(c1, c2);

    return BE_shape_2D_collider_GJK_simplex_contains_origin(s, collision_info);
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
static vector2_t BE_shape_2D_collider_GJK_support_function(basilisk_entity *c1, basilisk_entity *c2, vector2_t direction)
{
    return vector2_substract(shape_2D_collider_support(c1, direction), shape_2D_collider_support(c2, vector2_negate(direction)));
}

/**
 * @brief Creates the optimal simplex of the Minkowski substraction of the shape c1 and the negation of the shape c2.
 * This simplex is guaranteed to contain the origin of the space this Minkowski substraction lives into.
 *
 * @param[in] c1 first shape collider, must have a parent shape
 * @param[in] c2 second shape collider, must have a parent shape
 * @return simplex
 */
static simplex BE_shape_2D_collider_GJK_create_simplex(basilisk_entity *c1, basilisk_entity *c2)
{
    vector2_t direction = VECTOR2_ZERO;
    vector2_t normal_C0 = VECTOR2_ZERO;
    vector2_t tmp = { 0u };
    simplex returned_simplex = { 0u };
    size_t nb_iteration = 0u;
    bool simplex_needs_optimisation = false;

    if (!c1 || !c2) {
        return (simplex) { 0u };
    }

    // choosing a vector perpendicular to the shapes' direction to each other has a good chance to only need one iteration
    direction = vector2_normal_of(vector2_substract(
            body_2D_global(shape_2D_collider_get_body(c2)).position,
            body_2D_global(shape_2D_collider_get_body(c1)).position));
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
    while (simplex_needs_optimisation || (nb_iteration > GJK_ITERATION_LIMIT)) {
        simplex_needs_optimisation = false;

        normal_C0 = vector2_normalize(vector2_substract(VECTOR2_ORIGIN, returned_simplex.C));

        if (vector2_dot_product(BE_shape_2D_collider_segment_normal_opposite(returned_simplex.C, returned_simplex.A, returned_simplex.B), normal_C0) > 0.f) {
            returned_simplex.A = returned_simplex.B;
            returned_simplex.B = returned_simplex.C;
            simplex_needs_optimisation = true;
        } else if (vector2_dot_product(BE_shape_2D_collider_segment_normal_opposite(returned_simplex.C, returned_simplex.B, returned_simplex.A), normal_C0) > 0.f) {
            returned_simplex.B = returned_simplex.A;
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

        nb_iteration += 1u;
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
    vector2_t normal_C0 = VECTOR2_ZERO;
    vector2_t normal_CA = VECTOR2_ZERO;
    vector2_t normal_CB = VECTOR2_ZERO;
    bool colliding = false;

    normal_C0 = vector2_normalize(vector2_substract(VECTOR2_ORIGIN, tested.C));
    normal_CA = BE_shape_2D_collider_segment_normal_opposite(tested.C, tested.A, tested.B);
    normal_CB = BE_shape_2D_collider_segment_normal_opposite(tested.C, tested.B, tested.A);

    colliding = (vector2_dot_product(normal_C0, normal_CA) < 0.f) && (vector2_dot_product(normal_C0, normal_CB) < 0.f);

    if (colliding && collision_info) {
        *collision_info = (collision_2D_info) {
                .normal = (vector2_euclidian_norm(tested.A) > vector2_euclidian_norm(tested.B)) ? normal_CB : normal_CA,
        };
    }

    return colliding;
}

/**
 * @brief
 *
 * @param start
 * @param end
 * @param control
 * @return
 */
static vector2_t BE_shape_2D_collider_segment_normal_opposite(vector2_t start, vector2_t end, vector2_t control)
{
    return vector2_normalize(vector2_triple_product(
            vector2_substract(control, start),
            vector2_substract(end, start),
            vector2_substract(end, start)));
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static void BE_collision_manager_2D_GJK_draw_simplex(simplex s, SDL_Renderer *renderer)
{
    if (!renderer) {
        return;
    }

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawRect(renderer, &(const SDL_Rect) { .x = 298, .y = 148, .w = 4, .h = 4 });
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDrawLineF(renderer, s.A.x + 300, s.A.y + 150, s.B.x + 300, s.B.y + 150);
    SDL_SetRenderDrawColor(renderer, 255,   0,   0, 255);
    SDL_RenderDrawLineF(renderer, s.B.x + 300, s.B.y + 150, s.C.x + 300, s.C.y + 150);
    SDL_SetRenderDrawColor(renderer,   0, 255,   0, 255);
    SDL_RenderDrawLineF(renderer, s.C.x + 300, s.C.y + 150, s.A.x + 300, s.A.y + 150);
}