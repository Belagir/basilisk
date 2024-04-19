
#include <tarasque.h>

#include "collisionfest/app.h"

int main(void) {
    tarasque_engine *engine = tarasque_engine_create();

    tarasque_entity_add_child(tarasque_engine_root_entity(engine), "app", (tarasque_specific_entity) {
            .data = NULL,
            .entity_def = application_def,
    });

    tarasque_engine_run(engine, 120);

    tarasque_engine_destroy(&engine);

    return 0;
}
