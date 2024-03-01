
#include <tarasque.h>

int main(void)
{
    tarasque_engine *handle = tarasque_engine_create();


    tarasque_engine_destroy(&handle);

    return 0;
}
