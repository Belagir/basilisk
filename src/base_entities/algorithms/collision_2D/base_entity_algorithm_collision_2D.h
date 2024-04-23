/**
 * @file base_entity_algorithm_collision_2D.h
 * @author gabriel ()
 * @brief Interface to collision detection algorithms.
 * Currently implemented : narrow phase 2D GJK algorithm.
 *
 * @version 0.1
 * @date 2024-04-22
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef __BASE_ENTITY_ALGORITHM_COLLISION_2D_H__
#define __BASE_ENTITY_ALGORITHM_COLLISION_2D_H__

#include <base_entities/sdl_entities.h>

/* Checks the collision between two shape colldiers with the GJK algorithm. */
bool BE_collision_manager_2D_GJK_check(BE_shape_2D_collider *c1, BE_shape_2D_collider *c2, collision_2D_info *collision_info);


#endif
