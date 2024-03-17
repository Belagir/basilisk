
#ifndef __GRAFTING_H__
#define __GRAFTING_H__

#include <tarasque.h>

#include "../common.h"

typedef graft_user_data graft_user_data_copy;

graft_user_data_copy graft_user_data_copy_create(graft_user_data data, allocator alloc);

void graft_user_data_copy_destroy(graft_user_data *data, allocator alloc);

#endif
