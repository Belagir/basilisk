
#include "common.h"

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