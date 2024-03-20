
#ifndef __GRAFTING_H__
#define __GRAFTING_H__

#include <tarasque.h>

#include "../common.h"

typedef tarasque_specific_graft graft_user_data_copy;

graft_user_data_copy graft_user_data_copy_create(tarasque_specific_graft data, allocator alloc);

void graft_user_data_copy_destroy(tarasque_specific_graft *data, allocator alloc);

#endif
