/**
 * @file grating.c
 * @author gabriel ()
 * @brief
 * @version 0.1
 * @date 2024-03-23
 *
 * @copyright Copyright (c) 2024
 *
 */
#include "grafting.h"

/**
 * @brief
 *
 * @param data
 * @param alloc
 * @return
 */
tarasque_specific_graft_copy graft_user_data_copy_create(tarasque_specific_graft data, allocator alloc)
{
    tarasque_specific_graft_copy copy = { 0u };

    copy.graft_procedure = data.graft_procedure;

    if (data.args && (data.args_size > 0u)) {
        copy.args = alloc.malloc(alloc, data.args_size);
        bytewise_copy(copy.args, data.args, data.args_size);
        copy.args_size = data.args_size;
    }

    return copy;
}

/**
 * @brief
 *
 * @param data
 * @param alloc
 */
void graft_user_data_copy_destroy(tarasque_specific_graft *data, allocator alloc)
{
    if (!data) {
        return;
    }

    if (data->args) {
        alloc.free(alloc, data->args);
    }

    *data = (tarasque_specific_graft) { 0u };
}
