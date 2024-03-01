
#include "common.h"

/*  */
static i32 range_identifier_compare_character(const void *lhs, const void *rhs);

/**
 * @brief
 *
 * @param lhs
 * @param rhs
 * @return i32
 */
i32 range_identifier_compare(const void *lhs, const void *rhs)
{
    range_identifier *name_lhs = { (range_identifier *) lhs };
    range_identifier *name_rhs = { (range_identifier *) rhs };

    return range_compare(&range_to_any(name_lhs), &range_to_any(name_rhs), &range_identifier_compare_character);
}

/**
 * @brief Compares two characters with their position on the ASCII table.
 *
 * @param lhs
 * @param rhs
 * @return i32
 */
static i32 range_identifier_compare_character(const void *lhs, const void *rhs)
{
    const char c_lhs = { *(const char *) lhs };
    const char c_rhs = { *(const char *) rhs };

    return (c_lhs > c_rhs) - (c_lhs < c_rhs);
}

/**
 * @brief 
 * 
 * @param str 
 * @param alloc 
 * @return 
 */
range_identifier *range_identifier_from_cstring(const char *str, allocator alloc)
{
    size_t str_length = 0u;

    if (!str) {
        return NULL;
    }

    while (str[str_length] != '\0') {
        str_length += 1;
    }

    return range_create_dynamic_from(alloc, sizeof(*str), str_length, str_length, str);
}
