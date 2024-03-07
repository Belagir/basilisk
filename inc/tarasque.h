/**
 * @file tarasque.h
 * @author gabriel ()
 * @brief This is the main header containing the interface to the tarasque engine. This is normally
 * all the user needs to create an instance of the engine, add entities and organize interactions
 * between them.
 *
 * @version 0.1
 * @date 2024-03-06
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef __TARASQUE_H__
#define __TARASQUE_H__

#include <stddef.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* Opaque type of an engine handle. Those are used to carry around references to the engine's data. */
typedef struct tarasque_engine tarasque_engine;

// -------------------------------------------------------------------------------------------------

/**
 * @brief Data representing the key components of an entity.
 */
typedef struct entity_user_data {
    /** Size, in bytes, of the entity's specific data. */
    size_t data_size;
    /** Pointer (can be null) to some entity-specific data. This data is copied to the engine by
    functions that take the containing struct type. */
    void *data;

    /** Function ran on the entity-specific data when it is first created. */
    void (*on_init)(void *entity_data, tarasque_engine *handle);
    /** Function ran on the entity-specific data when it is destroyed. */
    void (*on_deinit)(void *entity_data, tarasque_engine *handle);
    /** Function ran on the entity-specific data each frame. */
    void (*on_frame)(void *entity_data, float elapsed_ms, tarasque_engine *handle);
} entity_user_data;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* Creates an instance of the engine on the heap. */
tarasque_engine *tarasque_engine_create(void);
/* Destroys a previously allocated engine instance and nullifies the given pointer. */
void tarasque_engine_destroy(tarasque_engine **handle);

// -------------------------------------------------------------------------------------------------

/* Starts the main loop of the engine, resolving pending operations, sending events and stepping
entities. */
void tarasque_engine_run(tarasque_engine *handle, int fps);
/* Flags the engine to exit next frame. */
void tarasque_engine_quit(tarasque_engine *handle);

// -------------------------------------------------------------------------------------------------

/* From an entity callback or not, adds a pending command to add another entity to the game tree. */
void tarasque_engine_add_entity(tarasque_engine *handle, const char *str_path, const char *str_id, entity_user_data user_data);
/* From an entity callback or not, adds a pending operation to remove an entity from the game tree. */
void tarasque_engine_remove_entity(tarasque_engine *handle, const char *str_path);

/* From an entity callback and only, adds a pending operation to subscribe a callback to an event by the event's name. */
void tarasque_engine_subscribe_to_event(tarasque_engine *handle, const char *str_event_name, void (*callback)(void *entity_data, void *event_data));
/* From an entity or not, detached from it or not, sends an event to subscribed entities. */
void tarasque_engine_stack_event(tarasque_engine *handle, const char *str_event_name, size_t event_data_size, void *event_data, bool is_detached);

#endif
