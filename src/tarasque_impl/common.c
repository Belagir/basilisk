/**
 * @file common.c
 * @author gabriel ()
 * @brief Implemetation details of common utilities.
 * @version 0.1
 * @date 2024-03-08
 *
 * @copyright Copyright (c) 2024
 *
 */
#include <stdio.h>

#include "common.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static const range(const char, 1) identifier_root_impl = range_create_static_fit(const char, {'\0'});
const identifier *const identifier_root = (const identifier *const) &identifier_root_impl;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/*  */
static i32 identifier_compare_character(const void *lhs, const void *rhs);

/*  */
static identifier *identifier_create_base(const char *str, allocator alloc, bool keep_terminator);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Allocates an identifier that copies the contents of a NULL-terminated string.
 *
 * @param[in] str NULL-terminated string to copy
 * @param[inout] alloc Allocator used for the copy.
 * @return identifier *
 */
identifier *identifier_from_cstring(const char *str, allocator alloc)
{
    identifier *new_id = identifier_create_base(str, alloc, true);
    bool is_id_allowed = (range_count(range_to_any(new_id), &identifier_compare_character, &(const char) { '/' },  0u) == 0u);

    if (!is_id_allowed) {
        range_destroy_dynamic(alloc, &range_to_any(new_id));
    }

    return new_id;
}

/**
 * @brief Allocates a path from a NULL-terminated string of names separated by '/'.
 *
 * @param[in] str NULL-terminated string to copy
 * @param[inout] alloc Allocator used for the copy.
 * @return path *
 */
path *path_from_cstring(const char *str, allocator alloc)
{
    identifier *source_string = { 0u };
    size_t start_of_token = 0u;
    size_t end_of_token = 0u;
    identifier *token = { 0u };
    path *new_path = { 0u };

    if (!str) {
        return NULL;
    }

    // allocating read data & output data
    source_string = identifier_create_base(str, alloc, false);
    new_path = range_create_dynamic(alloc, sizeof(*new_path->data), TARASQUE_COLLECTIONS_START_LENGTH);

    // search for tokens
    while (end_of_token < source_string->length) {
        end_of_token = range_index_of(
                range_to_any(source_string),
                &identifier_compare_character,
                &(const char) { '/' },
                end_of_token + 1);

        if (start_of_token < end_of_token) {
            token = range_create_dynamic_from(alloc, sizeof(*token->data), (end_of_token - start_of_token) + 1u, (end_of_token - start_of_token), source_string->data + start_of_token);
            range_insert_value(range_to_any(token), token->length, &(const char) { '\0' });

            if (token) {
                new_path = range_ensure_capacity(alloc, range_to_any(new_path), 1);
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
 * @brief Utility to bulk remove indentifiers stored in a path.
 *
 * @param[in] path Path to destroy.
 * @param[inout] alloc Allocator used for the free.
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

// -------------------------------------------------------------------------------------------------

/**
 * @brief Prints an indentifier to stdout.
 *
 * @param id
 */
void print_identifier(const identifier *id)
{
    if (!id) {
        return;
    }

    for (size_t i = 0u ; i < id->length ; i++) {
        printf("%c", id->data[i]);
    }
}

/**
 * @brief Prints a path to stdout.
 *
 * @param p
 */
void print_path(const path *p)
{
    if (!p) {
        return;
    }

    for (size_t i = 0u ; i < p->length ; i++) {
        print_identifier(p->data[i]);
        if ((i + 1) != p->length) {
            printf(" / ");
        }
    }
    printf("\n");
}

// -------------------------------------------------------------------------------------------------

/**
 * @brief Compares two identifiers hidden between two indirections (such as a double pointer to an entity, that stores an identifier -- that is always accessed through a pointer since it is an incomplete type)
 *
 * @param lhs
 * @param rhs
 * @return i32
 */
i32 identifier_compare_doubleref(const void *lhs, const void *rhs)
{
    // FIXME : triple pointer ???  because the comparator must receive a pointer to whatever is stored in the range, ie a pointer to an entity, that holds a pointer to an identifier........
    identifier **name_lhs = { *(identifier ***) lhs };
    identifier **name_rhs = { *(identifier ***) rhs };

    return identifier_compare(name_lhs, name_rhs);
}

/**
 * @brief Compares two identifiers.
 *
 * @param lhs
 * @param rhs
 * @return
 */
i32 identifier_compare(const void *lhs, const void *rhs)
{
    identifier *name_lhs = { *(identifier **) lhs };
    identifier *name_rhs = { *(identifier **) rhs };

    return range_compare(&range_to_any(name_lhs), &range_to_any(name_rhs), &identifier_compare_character);
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

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
 * @param keep_terminator
 * @return
 */
static identifier *identifier_create_base(const char *str, allocator alloc, bool keep_terminator)
{
    size_t str_length = 0u;
    identifier *new_identifier = NULL;

    if (!str) {
        return NULL;
    }

    while (str[str_length] != '\0') {
        str_length += 1;
    }

    str_length += (size_t) keep_terminator;

    new_identifier = range_create_dynamic_from(alloc, sizeof(*str), str_length, str_length, str);

    return new_identifier;
}