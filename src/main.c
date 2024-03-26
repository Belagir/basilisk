
#include <tarasque_bare.h>

#include "side_shooty/side_shooty.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

int main(void)
{
    tarasque_engine *handle = tarasque_engine_create();

    (void) tarasque_entity_add_child(tarasque_engine_root_entity(handle), "Game", side_shooty_game());

    tarasque_engine_run(handle, 60);

    tarasque_engine_destroy(&handle);

    return 0;
}
