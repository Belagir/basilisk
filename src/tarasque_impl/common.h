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
 * @brief String of characters representing a name.
 */
typedef RANGE(char) identifier;

/**
 * @brief Array of identifiers representing a path of entities.
 */
typedef RANGE(identifier *) path;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* Allocates an identifier that copies the contents of a NULL-terminated string. */
identifier *identifier_from_cstring(const char *str, allocator alloc);

/* Allocates a path from a NULL-terminated string of names separated by '/'. */
path *path_from_cstring(const char *str, allocator alloc);

/* Utility to bulk remove indentifiers stored in a path. */
void path_destroy(path **p, allocator alloc);

// -------------------------------------------------------------------------------------------------

void identifier_increment(identifier **base, allocator alloc);

// -------------------------------------------------------------------------------------------------

/* Prints an indentifier to stdout. */
void print_identifier(const identifier *id);

/* Prints a path to stdout. */
void print_path(const path *p);

// -------------------------------------------------------------------------------------------------

i32 identifier_compare_to_cstring(const identifier *id, const char *str);

/* Compares two identifiers hidden between two indirections. */
i32 identifier_compare_doubleref(const void *lhs, const void *rhs);

/* Compares two identifiers. */
i32 identifier_compare(const void *lhs, const void *rhs);

// -------------------------------------------------------------------------------------------------

bool character_is_num(char c);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* Constant used to quickly access the unique identifier of the root entity. */
extern const identifier *const identifier_root;

#endif
