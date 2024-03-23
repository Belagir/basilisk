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
#include <math.h>

#include "common.h"


// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static const RANGE(const char, 1) identifier_root_impl = RANGE_CREATE_STATIC_FIT(const char, {'\0'});
const identifier *const identifier_root = (const identifier *const) &identifier_root_impl;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* Comapares two characters by their position in the ASCII table. */
static i32 identifier_compare_character(const void *lhs, const void *rhs);

/* Versatile utility to create identifiers from NULL-terminated strings with finer control. */
static identifier *identifier_create_base(const char *str, allocator alloc, bool keep_terminator);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Allocates an identifier that copies the contents of a NULL-terminated string.
 * The function will return NULL if the identifier is empty or contains at least one '/'.
 *
 * @param[in] str NULL-terminated string to copy
 * @param[inout] alloc Allocator used for the copy.
 * @return identifier *
 */
identifier *identifier_from_cstring(const char *str, allocator alloc)
{
    identifier *new_id = identifier_create_base(str, alloc, true);
    bool is_id_allowed = (new_id->length > 0u) && (range_count(RANGE_TO_ANY(new_id), &identifier_compare_character, &(const char) { '/' },  0u) == 0u);

    if (!is_id_allowed) {
        range_destroy_dynamic(alloc, &RANGE_TO_ANY(new_id));
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
        end_of_token = range_index_of(RANGE_TO_ANY(source_string), &identifier_compare_character, &(const char) { '/' }, end_of_token + 1);

        if (start_of_token < end_of_token) {
            token = range_create_dynamic_from(alloc, sizeof(*token->data), (end_of_token - start_of_token) + 1u, (end_of_token - start_of_token), source_string->data + start_of_token);
            range_insert_value(RANGE_TO_ANY(token), token->length, &(const char) { '\0' });

            if (token) {
                new_path = range_ensure_capacity(alloc, RANGE_TO_ANY(new_path), 1);
                (void) range_insert_value(RANGE_TO_ANY(new_path), new_path->length, &token);
            }
        }

        start_of_token = end_of_token + 1;
    }

    // cleanup
    range_destroy_dynamic(alloc, &RANGE_TO_ANY(source_string));

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
        range_destroy_dynamic(alloc, &RANGE_TO_ANY((*p)->data[i]));
    }

    range_destroy_dynamic(alloc, &RANGE_TO_ANY(*p));
    *p = NULL;
}

// -------------------------------------------------------------------------------------------------

/**
 * @brief Increments a the trailing number of an identifier by one.
 * If there is no number at the end of an identifier, then a '1' is appended at the end.
 *
 * @param[inout] base Pointer to an identifier to increment.
 * @param[inout] alloc Allocator used for the potential extension of the identifier.
 */
void identifier_increment(identifier **base, allocator alloc)
{
    i64 index = 0u;

    if (!base || !*base) {
        return;
    }

    index = (i64) (*base)->length - 1;
    while ((index > 0) && ((*base)->data[index - 1] == '9')) {
        (*base)->data[index - 1] = '0';
        index -= 1;
    }

    if (character_is_num((*base)->data[index - 1])) {
        (*base)->data[index - 1] += 1;
    } else {
        *base = range_ensure_capacity(alloc, RANGE_TO_ANY(*base), 1);
        range_insert_value(RANGE_TO_ANY(*base), (size_t) index, &(char) { '1' });
    }
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
 * @brief Compares an identifier to a NULL-terminated string.
 *
 * @param id
 * @param str
 * @return i32
 */
i32 identifier_compare_to_cstring(const identifier *id, const char *str)
{
    size_t pos = 0u;
    i32 cmp_result = 0;

    while((pos < id->length) && (str[pos] != '\0') && (cmp_result == 0)) {
        cmp_result = identifier_compare_character(str + pos, id->data + pos);
        pos += 1u;
    }

    return cmp_result;
}

/**
 * @brief Compares two identifiers hidden between two indirections (such as a double pointer to an entity, that stores an identifier -- that is always accessed through a pointer since it is an incomplete type)
 *
 * @param lhs
 * @param rhs
 * @return i32
 */
i32 identifier_compare_doubleref(const void *lhs, const void *rhs)
{
    // FIXME (low priority because of possible big impact on implementation) : triple pointer ???  because the comparator must receive a pointer to whatever is stored in the range, ie a pointer to an entity, that holds a pointer to an identifier........
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

    return range_compare(&RANGE_TO_ANY(name_lhs), &RANGE_TO_ANY(name_rhs), &identifier_compare_character);
}

// -------------------------------------------------------------------------------------------------

bool character_is_num(char c)
{
    return (c >= '0') && (c <= '9');
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
 * @brief Creates an indentifier from a araw string with some control over the output.
 *
 * @param[in] str Source null terminated string.
 * @param[inout] alloc Allocator used to create the identifier and to copy the source string.
 * @param[in] keep_terminator If enabled, the identifier will contain the terminating character the string '\0'.
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