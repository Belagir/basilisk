
#include <tarasque.h>

int main(void)
{
    tarasque_engine *handle = tarasque_engine_create();

    tarasque_engine_add_entity(handle, "", "test", (entity_template) { 0u });

    tarasque_engine_destroy(&handle);

    return 0;
}
