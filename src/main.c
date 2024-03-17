
#include <stdio.h>

#include <tarasque.h>
#include <grafts/sdl2_window.h>

int main(void)
{
    tarasque_engine *handle = tarasque_engine_create();


    tarasque_engine_run(handle, 60);

    tarasque_engine_destroy(&handle);

    return 0;
}
