
#include <tarasque.h>

int main(void)
{
    tarasque_engine *handle = tarasque_engine_create();

    tarasque_engine_add_entity(handle, "", "test", (entity_template) { 0u });
    tarasque_engine_add_entity(handle, "test", "f", (entity_template) { 0u });
    tarasque_engine_add_entity(handle, "test", "c", (entity_template) { 0u });
    tarasque_engine_add_entity(handle, "test", "e", (entity_template) { 0u });
    tarasque_engine_add_entity(handle, "test", "a", (entity_template) { 0u });
    tarasque_engine_add_entity(handle, "test", "b", (entity_template) { 0u });
    tarasque_engine_add_entity(handle, "test", "d", (entity_template) { 0u });

    tarasque_engine_run(handle, 60);

    tarasque_engine_destroy(&handle);

    return 0;
}
