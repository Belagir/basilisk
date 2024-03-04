
#ifndef __TARASQUE_H__
#define __TARASQUE_H__

#include <stddef.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/*  */
typedef struct tarasque_engine tarasque_engine;

// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 */
typedef struct entity_template {
    size_t data_size;
    void *data;

    void (*on_init)(void *entity_data, tarasque_engine *handle);
    void (*on_deinit)(void *entity_data, tarasque_engine *handle);
    void (*on_frame)(void *entity_data, float elapsed_ms, tarasque_engine *handle);
} entity_template;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/*  */
tarasque_engine *tarasque_engine_create(void);
/*  */
void tarasque_engine_destroy(tarasque_engine **handle);

// -------------------------------------------------------------------------------------------------

/*  */
void tarasque_engine_run(tarasque_engine *handle, int fps);
/*  */
void tarasque_engine_quit(tarasque_engine *handle);

// -------------------------------------------------------------------------------------------------

/*  */
void tarasque_engine_add_entity(tarasque_engine *handle, const char *str_path, const char *str_id, entity_template template);
/*  */
void tarasque_engine_remove_entity(tarasque_engine *handle, const char *str_path);

/*  */
void tarasque_engine_subscribe_to_event(tarasque_engine *handle, const char *str_id, void (*callback)(void *entity_data, void *event_data));
/*  */
void tarasque_engine_stack_event(tarasque_engine *handle, const char *str_event_name, size_t event_data_size, void *event_data, bool is_detached);

#endif
