/**
 * @file tarasque_bare.h
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
#ifndef __TARASQUE_BARE_H__
#define __TARASQUE_BARE_H__

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// STATIC CONSTANTS

#ifndef TARASQUE_RESOURCE_STORAGES_FOLDER
#define TARASQUE_RESOURCE_STORAGES_FOLDER "program_data"
#endif

#ifndef TARASQUE_RESOURCE_STORAGES_EXTENSION
#define TARASQUE_RESOURCE_STORAGES_EXTENSION "data"
#endif

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// INTERFACE TYPES

/* Opaque type of an engine handle. Those are used to carry around references to the engine's data. */
typedef struct tarasque_engine tarasque_engine;

/* Anonymous type to whatever the user chose for an entity to store. Used as an access to change the state of an entity and its children. */
typedef void tarasque_entity;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// CONSTRUCTOR DATA STRUCTURES

typedef struct tarasque_entity_definition tarasque_entity_definition;
typedef struct tarasque_entity_definition {
    /** Size, in bytes, of the entity's specific data. */
    unsigned long data_size;

    /** Function ran on the entity-specific data when it is first created. */
    void (*on_init)(tarasque_entity *self_data);
    /** Function ran on the entity-specific data when it is destroyed. */
    void (*on_deinit)(tarasque_entity *self_data);
    /** Function ran on the entity-specific data each frame. */
    void (*on_frame)(tarasque_entity *self_data, float elapsed_ms);

    const tarasque_entity_definition *subtype;
} tarasque_entity_definition;

/**
 * @brief Data representing the key components of an entity.
 */
typedef struct tarasque_specific_entity {
    /**  */
    tarasque_entity_definition entity_def;
    /** Pointer (can be null) to some entity-specific data. This data is copied to the engine by
    functions that take the containing struct type. */
    tarasque_entity *data;
} tarasque_specific_entity;

/**
 * @brief Data representing how to change the game tree to accomodate for a specific graft.
 */
typedef struct tarasque_specific_graft {
    /** Size, in bytes, of the graft's arguments. */
    unsigned long args_size;
    /** Pointer (can be null) to the graft's arguments data. This data is copied to the engine by
    functions that take the containing struct type. */
    void *args;

    /** Procedure called to add the graft. */
    void (*graft_procedure)(tarasque_entity *entity, void *graft_args);
} tarasque_specific_graft;

/**
 * @brief Data representing the behavior to of an entity on receiving an event.
 */
typedef struct tarasque_specific_event_subscription {
    /** Relative priority (to other callbacks) of the callback when receiving an event. The higher, the later the callback will receive the event. */
    int index;
    /** Fucntion executed when an event is received. */
    void (*callback)(tarasque_entity *self_data, void *event_data);
} tarasque_specific_event_subscription;

/**
 * @brief Data representing a new event to add to the stack.
 */
typedef struct tarasque_specific_event {
    /** If set, the event will not be removed if the entity that sent it is removed itself. */
    bool is_detached;

    /** Size, in bytes, of the event's data.*/
    unsigned long data_size;
    /** The event's specific data. */
    void *data;
} tarasque_specific_event;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// ---- INTERFACE METHODS

// -------------------------------------------------------------------------------------------------
// LIFETIME FUNCTIONS

/* Creates an instance of the engine on the heap. */
tarasque_engine *tarasque_engine_create(void);
/* Destroys a previously allocated engine instance and nullifies the given pointer. */
void tarasque_engine_destroy(tarasque_engine **handle);

// -------------------------------------------------------------------------------------------------
// ENGINE METHODS

/* Returns the data bound to the root entity. This pointer has no memory behind it and should not be dereferenced. */
tarasque_entity *tarasque_engine_root_entity(tarasque_engine *handle);
/* Starts the main loop of the engine, resolving pending commands, sending events and stepping
entities. */
void tarasque_engine_run(tarasque_engine *handle, int fps);

// -------------------------------------------------------------------------------------------------
// ENTITY INTERACTIONS

/* Flags the engine to exit next frame. */
void tarasque_entity_quit(tarasque_entity *entity);
/* Adds a pending command to subscribe a callback to an event, by the event's name. */
void tarasque_entity_queue_subscribe_to_event(tarasque_entity *entity, const char *str_event_name, tarasque_specific_event_subscription subscription_data);
/* Sends an event to subscribed entities. */
void tarasque_entity_stack_event(tarasque_entity *entity, const char *str_event_name, tarasque_specific_event event_data);

// -------------------------------------------------------------------------------------------------
// ENTITY HIERARCHY MODIFICATIONS

/* Adds another entity to the game tree as a child of another. */
tarasque_entity *tarasque_entity_add_child(tarasque_entity *entity, const char *str_id, tarasque_specific_entity user_data);
/* Adds a pending command to remove an entity from the game tree. */
void tarasque_entity_queue_remove(tarasque_entity *entity);
/* Realizes a graft in the game tree relative to an entity. */
void tarasque_entity_graft(tarasque_entity *entity, const char *str_id, tarasque_specific_graft graft_data);

// -------------------------------------------------------------------------------------------------
// ENTITY SEARCHING

/* Search for a parent of a certain name and / or definition an entity might be related to. If the name and definition are NULL, the first parent is returned. */
tarasque_entity *tarasque_entity_get_parent(tarasque_entity *entity, const char *str_parent_name, const tarasque_entity_definition *entity_def);
/* Search for a child entity located at a specific path relative to an entity, while optionally checking if it was created with a specific definition. */
tarasque_entity *tarasque_entity_get_child(tarasque_entity *entity, const char *str_path, const tarasque_entity_definition *entity_def);
/* Resolves wether or not the entity has been defined using a specific definition or was marked as subtyping it. */
bool tarasque_entity_is(tarasque_entity *entity, tarasque_entity_definition entity_def);


// -------------------------------------------------------------------------------------------------
// RESOURCE HANDLING

/* In updating mode (TARASQUE_UPDATE_RESOURCES set), this function will update the resource file from the filesystem to the storage file. */
/* Otherwise, this function will check that the resource is present in the given storage file and log what it finds. */
void tarasque_engine_declare_resource(tarasque_engine *handle, const char *str_storage, const char *str_file_path);
#define tarasque_engine_declare_resource(handle, str_storage, str_file_path) tarasque_engine_declare_resource(handle, TARASQUE_RESOURCE_STORAGES_FOLDER "/" str_storage "." TARASQUE_RESOURCE_STORAGES_EXTENSION, str_file_path)

/* Loads a storage file if not already and search for a resource in it, returning its data. */
void *tarasque_entity_fetch_resource(tarasque_entity *entity, const char *str_storage, const char *str_file_path, unsigned long *out_size);
#define tarasque_entity_fetch_resource(handle, str_storage, str_file_path, out_size) tarasque_entity_fetch_resource(handle, TARASQUE_RESOURCE_STORAGES_FOLDER "/" str_storage "." TARASQUE_RESOURCE_STORAGES_EXTENSION, str_file_path, out_size)

#endif
