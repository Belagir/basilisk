
#include "common.h"

/*  */
static i32 tarasque_string_compare_character(const void *lhs, const void *rhs);

/**
 * @brief
 *
 * @param lhs
 * @param rhs
 * @return i32
 */
i32 tarasque_string_compare(const void *lhs, const void *rhs)
{
    identifier *name_lhs = { (identifier *) lhs };
    identifier *name_rhs = { (identifier *) rhs };

    return range_compare(&range_to_any(name_lhs), &range_to_any(name_rhs), &tarasque_string_compare_character);
}

/**
 * @brief Compares two characters with their position on the ASCII table.
 *
 * @param lhs
 * @param rhs
 * @return i32
 */
static i32 tarasque_string_compare_character(const void *lhs, const void *rhs)
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
identifier *tarasque_string_from_cstring(const char *str, allocator alloc)
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

/**
 * @brief 
 * 
 * @param string 
 * @param alloc 
 */
void tarasque_string_destroy(identifier **string, allocator alloc)
{
    if (!string || !*string) {
        return;
    }

    range_destroy_dynamic(alloc, &range_to_any(*string));
    *string = NULL;
}

/**
 * @brief 
 * 
 * @param str 
 * @param alloc 
 * @return 
 */
path *tarasque_path_from_cstring(const char *str, allocator alloc)
{

}

/**
 * @brief 
 * 
 * @param path 
 * @param alloc 
 */
void tarasque_path_destroy(path **path, allocator alloc)
{

}