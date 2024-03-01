
#ifndef __TARASQUE_H__
#define __TARASQUE_H__

#include <stddef.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------


typedef struct tarasque_engine tarasque_engine;

// -------------------------------------------------------------------------------------------------

typedef struct entity_template {
    size_t data_size;
    void *data;

    void (*on_init)(void *data, tarasque_engine *handle);
    void (*on_deinit)(void *data, tarasque_engine *handle);
    void (*on_frame)(void *data, float elapsed_ms, tarasque_engine *handle);
} entity_template;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

tarasque_engine *tarasque_engine_create(void);
void tarasque_engine_destroy(tarasque_engine **handle);

// -------------------------------------------------------------------------------------------------

void tarasque_engine_run(tarasque_engine *handle, int fps);
void tarasque_engine_quit(tarasque_engine *handle);

// -------------------------------------------------------------------------------------------------

void tarasque_engine_add_entity(tarasque_engine *handle, char *str_path, char *str_id, entity_template template);

#endif
