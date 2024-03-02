
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
typedef range(identifier) path;

/*  */
identifier *tarasque_string_from_cstring(const char *str, allocator alloc);

/*  */
void tarasque_string_destroy(identifier **string, allocator alloc);

/*  */
path *tarasque_path_from_cstring(const char *str, allocator alloc);

/*  */
void tarasque_path_destroy(path **path, allocator alloc);

/*  */
i32 tarasque_string_compare(const void *lhs, const void *rhs);

#endif
