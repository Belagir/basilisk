
#include "grafting.h"

graft_user_data_copy graft_user_data_copy_create(tarasque_graft_specific_data data, allocator alloc)
{
    graft_user_data_copy copy = { 0u };

    copy.graft_procedure = data.graft_procedure;

    if (data.args && (data.args_size > 0u)) {
        copy.args = alloc.malloc(alloc, data.args_size);
        bytewise_copy(copy.args, data.args, data.args_size);
        copy.args_size = data.args_size;
    }

    return copy;
}

void graft_user_data_copy_destroy(tarasque_graft_specific_data *data, allocator alloc)
{
    if (!data) {
        return;
    }

    if (data->args) {
        alloc.free(alloc, data->args);
    }

    *data = (tarasque_graft_specific_data) { 0u };
}
