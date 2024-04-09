/**
 * @file common.h
 * @author gabriel ()
 * @brief Collection of transversal and lightweight utilities widely used in the project.
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

#include <tarasque_bare.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/// Starting number of items of all collections used by the engine.
#define TARASQUE_COLLECTIONS_START_LENGTH (8)

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief String of characters representing a name. The identifier contains a trailing '\0' to ease interop with stdlib.
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

/* Increments the trailing number behind an identifier. */
void identifier_increment(identifier **base, allocator alloc);

// -------------------------------------------------------------------------------------------------

/* Prints an indentifier to stdout. */
void print_identifier(const identifier *id);

/* Prints a path to stdout. */
void print_path(const path *p);

// -------------------------------------------------------------------------------------------------

/* Compares two pointers (themselves passed by pointers) as two unsigned integers. */
i32 raw_pointer_compare(const void *lhs, const void *rhs);

/* Compares an identifier to a simple NULL_terminated string. Useful to circumvent memory allocation. */
i32 identifier_compare_to_cstring(const identifier *id, const char *str);

/* Compares two identifiers hidden between two indirections. */
i32 identifier_compare_doubleref(const void *lhs, const void *rhs);

/* Compares two identifiers. */
i32 identifier_compare(const void *lhs, const void *rhs);

// -------------------------------------------------------------------------------------------------

/* Returns true if the given ASCII character is in the range 0-9. */
bool character_is_num(char c);

/* Greedily computes the length of a null-terminated string. */
size_t c_string_length(const char *str, bool keep_terminator);

// -------------------------------------------------------------------------------------------------

/* Simple hash function to hash anything. */
u32 hash_jenkins_one_at_a_time(const byte *key, size_t length, u32 seed);

/* Hashes an identifier with an abritrary hash function. */
u32 hash_indentifier(const identifier *id, u32 (*hash_function)(const byte *key, size_t length, u32 seed));

/* Hashes a path, identifier by indetifier, with an abritrary hash function. */
u32 hash_path(const path *p, u32 (*hash_function)(const byte *key, size_t length, u32 seed));

/* Compares two 4-bytes hashes as if they were unsigned integers. */
i32 hash_compare(const void *lhs, const void *rhs);

/* Compares two pointers to 4-bytes hashes as if they were unsigned integers. */
i32 hash_compare_doubleref(const void *lhs, const void *rhs);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* Constant used to quickly access the unique identifier of the root entity. */
extern const identifier *const identifier_root;

#endif
