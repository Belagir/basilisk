
#ifndef __TARASQUE_H__
#define __TARASQUE_H__

typedef struct tarasque_engine tarasque_engine;

tarasque_engine *tarasque_engine_create(void);
void tarasque_engine_destroy(tarasque_engine **handle);

#endif
