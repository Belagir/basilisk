
#include <stdio.h>

#include "common.h"


static const range(const char, 1) identifier_root_impl = range_create_static_fit(const char, {'\0'});
const identifier *const identifier_root = (const identifier *const) &identifier_root_impl;

/*  */
static i32 identifier_compare_character(const void *lhs, const void *rhs);

/**
 * @brief Compares two characters with their position on the ASCII table.
 *
 * @param lhs
 * @param rhs
 * @return i32
 */
static i32 identifier_compare_character(const void *lhs, const void *rhs)
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
identifier *identifier_from_cstring(const char *str, allocator alloc)
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
 * @param str 
 * @param alloc 
 * @return 
 */
path *path_from_cstring(const char *str, allocator alloc)
{
    identifier *source_string = { 0u };
    size_t start_of_token = 0u;
    size_t end_of_token = 1u;
    identifier *token = { 0u };
    path *new_path = { 0u };

    if (!str) {
        return NULL;
    }

    // allocating read data & output data
    source_string = identifier_from_cstring(str, alloc);
    new_path = range_create_dynamic(alloc, sizeof(*new_path->data), TARASQUE_COLLECTIONS_START_SIZE);

    // search for tokens
    while (end_of_token < source_string->length) {
        end_of_token = range_index_of(
                range_to_any(source_string),
                &identifier_compare_character,
                &(const char) { '/' },
                end_of_token + 1);
        
        if (start_of_token != end_of_token) {
            token = range_create_dynamic_from_subrange_of(alloc, range_to_any(source_string), start_of_token, end_of_token);

            if (token) {
                new_path = range_ensure_capacity(alloc, range_to_any(new_path));
                (void) range_insert_value(range_to_any(new_path), new_path->length, &token);
            }
        }

        start_of_token = end_of_token + 1;
    }

    // cleanup
    range_destroy_dynamic(alloc, &range_to_any(source_string));

    return new_path;
}

/**
 * @brief 
 * 
 * @param path 
 * @param alloc 
 */
void path_destroy(path **p, allocator alloc)
{
    if (!p || !*p) {
        return;
    }

    for (size_t i = 0u ; i < (*p)->length ; i++) {
        range_destroy_dynamic(alloc, &range_to_any((*p)->data[i]));
    }

    range_destroy_dynamic(alloc, &range_to_any(*p));
    *p = NULL;
}

/**
 * @brief 
 * 
 * @param p 
 */
void print_path(const path *p)
{
    if (!p) {
        return;
    }

    for (size_t i = 0u ; i < p->length ; i++) {
        printf("%s", p->data[i]->data);
        if ((i + 1) != p->length) {
            printf(" / ");
        }
    }
    printf("\n");
}

/**
 * @brief
 *
 * @param lhs
 * @param rhs
 * @return i32
 */
i32 identifier_compare(const void *lhs, const void *rhs)
{
    identifier *name_lhs = { (identifier *) lhs };
    identifier *name_rhs = { (identifier *) rhs };

    return range_compare(&range_to_any(name_lhs), &range_to_any(name_rhs), &identifier_compare_character);
}
