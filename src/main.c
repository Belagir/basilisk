
#include <stdio.h>

#include <tarasque.h>

static void dummy_entity_init(void *data, tarasque_engine *handle) {
    printf("dummy n°%d initialized !\n", *(int *) data);
}

static void dummy_entity_deinit(void *data, tarasque_engine *handle) {
    printf("dummy n°%d deinitialized.\n", *(int *) data);
}

static void dummy_entity_step(void *data, float elapsed_ms, tarasque_engine *handle) {
    printf("dummy n°%d is doing something...\n", *(int *) data);
}

static void dummy_setup(tarasque_engine *handle) {
    tarasque_engine_add_entity(handle, "", "1", (entity_template) { 
            .data      = &(int) { 1 },
            .data_size = sizeof(int),
            .on_init   = &dummy_entity_init,
            .on_deinit = &dummy_entity_deinit,
            .on_frame  = &dummy_entity_step
    });
    tarasque_engine_add_entity(handle, "1", "2", (entity_template) { 
            .data      = &(int) { 2 },
            .data_size = sizeof(int),
            .on_init   = &dummy_entity_init,
            .on_deinit = &dummy_entity_deinit,
            .on_frame  = &dummy_entity_step
    });
    tarasque_engine_add_entity(handle, "1/2", "3", (entity_template) { 
            .data      = &(int) { 3 },
            .data_size = sizeof(int),
            .on_init   = &dummy_entity_init,
            .on_deinit = &dummy_entity_deinit,
            .on_frame  = &dummy_entity_step
    });
}

int main(void)
{
    tarasque_engine *handle = tarasque_engine_create();

    dummy_setup(handle);

    tarasque_engine_run(handle, 60);

    tarasque_engine_destroy(&handle);

    return 0;
}
