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

/* Anonymous type to whatever the user chose for an entity to store. Used as an access to change the state of an entity and its children. */
typedef void entity_data;

// -------------------------------------------------------------------------------------------------

/**
 * @brief Collection of callbacks that make an entity function in the engine.
 */
typedef struct entity_callbacks {
    /** Function ran on the entity-specific data when it is first created. */
    void (*on_init)(entity_data *self_data);
    /** Function ran on the entity-specific data when it is destroyed. */
    void (*on_deinit)(entity_data *self_data);
    /** Function ran on the entity-specific data each frame. */
    void (*on_frame)(entity_data *self_data, float elapsed_ms);
} entity_callbacks;

/**
 * @brief Data representing the key components of an entity.
 */
typedef struct entity_user_data {
    /** Size, in bytes, of the entity's specific data. */
    size_t data_size;
    /** Pointer (can be null) to some entity-specific data. This data is copied to the engine by
    functions that take the containing struct type. */
    entity_data *data;

    /** Specific callbacks. */
    entity_callbacks callbacks;
} entity_user_data;

/**
 * @brief Data representing the behavior to realize on an event.
 */
typedef struct event_subscription_user_data {
    void (*callback)(entity_data *self_data, void *event_data);
} event_subscription_user_data;

/**
 * @brief Data representing how to change the game tree to accomodate for a specific graft.
 */
typedef struct graft_user_data {
    /** Size, in bytes, of the graft's arguments. */
    size_t args_size;
    /** Pointer (can be null) to the graft's arguments data. This data is copied to the engine by
    functions that take the containing struct type. */
    void *args;

    /** procedure called to add the graft. */
    void (*graft_procedure)(entity_data *entity, void *graft_args);
} graft_user_data;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* Creates an instance of the engine on the heap. */
tarasque_engine *tarasque_engine_create(void);
/* Destroys a previously allocated engine instance and nullifies the given pointer. */
void tarasque_engine_destroy(tarasque_engine **handle);

// -------------------------------------------------------------------------------------------------

/* Returns the data bound to the root entity. This pointer has no memory behind it and cannot be dereferenced. */
entity_data *tarasque_engine_root_entity(tarasque_engine *handle);

/* Starts the main loop of the engine, resolving pending commands, sending events and stepping
entities. */
void tarasque_engine_run(tarasque_engine *handle, int fps);

// -------------------------------------------------------------------------------------------------

/* Flags the engine to exit next frame. */
void tarasque_entity_quit(entity_data *entity);

/* From or out of an entity callback, adds a pending command to add another entity to the game tree. */
void tarasque_entity_add_child(entity_data *entity, const char *str_path, const char *str_id, entity_user_data user_data);
/* From or out of an entity callback, adds a pending command to remove an entity from the game tree. */
void tarasque_entity_remove_child(entity_data *entity, const char *str_path);
/* From or out of an entity callback, adds a pending command to graft a set of entities in the game tree. */
void tarasque_entity_graft(entity_data *entity, const char *str_path, const char *str_id, graft_user_data graft_data);

/* From an entity callback and only, adds a pending command to subscribe a callback to an event by the event's name. */
void tarasque_entity_subscribe_to_event(entity_data *entity, const char *str_event_name, event_subscription_user_data subscription_data);
/* From an entity or not, detached from it or not, sends an event to subscribed entities. */
void tarasque_entity_stack_event(entity_data *entity, const char *str_event_name, size_t event_data_size, void *event_data, bool is_detached);

#endif
