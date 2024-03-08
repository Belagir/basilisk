/**
 * @file common.h
 * @author gabriel ()
 * @brief Collection of transversal utilities widely used in the project.
 * @version 0.1
 * @date 2024-03-07
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef __TARASQUE_COMMON_H__
#define __TARASQUE_COMMON_H__

#include <ustd/allocation.h>
#include <ustd/range.h>

#include <tarasque.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/// Starting number of items of all collections used by the engine.
#define TARASQUE_COLLECTIONS_START_LENGTH (1)

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 */
typedef range(const char) identifier;

/**
 * @brief
 */
typedef range(identifier *) path;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/*  */
identifier *identifier_from_cstring(const char *str, allocator alloc);

/*  */
path *path_from_cstring(const char *str, allocator alloc);

/*  */
void path_destroy(path **p, allocator alloc);

// -------------------------------------------------------------------------------------------------

/*  */
void print_identifier(const identifier *id);

/*  */
void print_path(const path *p);

// -------------------------------------------------------------------------------------------------

/*  */
i32 identifier_compare_tripleref(const void *lhs, const void *rhs);

/*  */
i32 identifier_compare(const void *lhs, const void *rhs);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/*  */
extern const identifier *const identifier_root;

#endif
