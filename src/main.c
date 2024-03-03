
#include <tarasque.h>

int main(void)
{
    tarasque_engine *handle = tarasque_engine_create();

    tarasque_engine_add_entity(handle, "", "an_entity", (entity_template) { 0u });
    tarasque_engine_add_entity(handle, "an_entity", "a_child", (entity_template) { 0u });
    tarasque_engine_add_entity(handle, "an_entity/a_child", "another_child", (entity_template) { 0u });

    tarasque_engine_run(handle, 60);

    tarasque_engine_destroy(&handle);

    return 0;
}
