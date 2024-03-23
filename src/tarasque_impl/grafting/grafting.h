/**
 * @file grafting.h
 * @author gabriel ()
 * @brief
 * @version 0.1
 * @date 2024-03-23
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef __GRAFTING_H__
#define __GRAFTING_H__

#include <tarasque.h>

#include "../common.h"

/**
 * @brief Redefinition of the user-filled tarasque_specific_graft type to signal memory allocation in case of copy.
 */
typedef tarasque_specific_graft tarasque_specific_graft_copy;

/* Copies graft data into an allocated object. */
tarasque_specific_graft_copy graft_user_data_copy_create(tarasque_specific_graft data, allocator alloc);

/* Releases memory held by an allocated copy. */
void graft_user_data_copy_destroy(tarasque_specific_graft *data, allocator alloc);

#endif
