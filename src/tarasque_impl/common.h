
#ifndef __TARASQUE_COMMON_H__
#define __TARASQUE_COMMON_H__

#include <ustd/allocation.h>
#include <ustd/range.h>

#include <tarasque.h>

#define TARASQUE_COLLECTIONS_START_SIZE (32)

/**
 * @brief
 *
 */
typedef range(const char) identifier;

/**
 * @brief
 *
 */
typedef range(identifier *) path;

/*  */
identifier *identifier_from_cstring(const char *str, allocator alloc);

/*  */
path *path_from_cstring(const char *str, allocator alloc);

/*  */
void path_destroy(path **p, allocator alloc);

/*  */
void print_identifier(const identifier *id);

/*  */
void print_path(const path *p);

/*  */
i32 identifier_compare(const void *lhs, const void *rhs);

extern const identifier *const identifier_root;

#endif
