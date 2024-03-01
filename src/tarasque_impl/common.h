
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
typedef range(const char) range_identifier;

/*  */
range_identifier *range_identifier_from_cstring(const char *str, allocator alloc);

/*  */
i32 range_identifier_compare(const void *lhs, const void *rhs);


#endif
