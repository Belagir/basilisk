
#ifndef __GRAFTING_H__
#define __GRAFTING_H__

#include <tarasque.h>

#include "../common.h"

typedef tarasque_graft_specific_data graft_user_data_copy;

graft_user_data_copy graft_user_data_copy_create(tarasque_graft_specific_data data, allocator alloc);

void graft_user_data_copy_destroy(tarasque_graft_specific_data *data, allocator alloc);

#endif
