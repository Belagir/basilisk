/**
 * @file basilisk_bare.h
 * @author gabriel ()
 * @brief This is the main header containing the interface to the basilisk engine. This is normally
 * all the user needs to create an instance of the engine, add entities and organize interactions
 * between them.
 *
 * @version 0.1
 * @date 2024-03-06
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef __BASILISK_H__
#define __BASILISK_H__

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// STATIC CONSTANTS

#ifndef BASILISK_RESOURCE_STORAGES_FOLDER
#define BASILISK_RESOURCE_STORAGES_FOLDER "program_data"
#endif

#ifndef BASILISK_RESOURCE_STORAGES_EXTENSION
#define BASILISK_RESOURCE_STORAGES_EXTENSION "data"
#endif

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// INTERFACE TYPES

/* Opaque type of an engine handle. Those are used to carry around references to the engine's data. */
typedef struct basilisk_engine basilisk_engine;

/* Anonymous type to whatever the user chose for an entity to store. Used as an access to change the state of an entity and its children. */
typedef void basilisk_entity;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// CONSTRUCTOR DATA STRUCTURES

/**
 * @brief Basic entity information. Contains expected callbacks and the data size the engine allocate for a "class" of entity.
 */
typedef struct basilisk_entity_definition basilisk_entity_definition;
typedef struct basilisk_entity_definition {
    /** Size, in bytes, of the entity's specific data. */
    unsigned long data_size;

    /** Function ran on the entity-specific data when it is first created. */
    void (*on_init)(basilisk_entity *self_data);
    /** Function ran on the entity-specific data when it is destroyed. */
    void (*on_deinit)(basilisk_entity *self_data);
    /** Function ran on the entity-specific data each frame. */
    void (*on_frame)(basilisk_entity *self_data, float elapsed_ms);
} basilisk_entity_definition;

/**
 * @brief Data representing the key components of an entity.
 */
typedef struct basilisk_specific_entity {
    /**  */
    basilisk_entity_definition entity_def;
    /** Pointer (can be null) to some entity-specific data. This data is copied to the engine by
    functions that take the containing struct type. */
    basilisk_entity *data;
} basilisk_specific_entity;

/**
 * @brief Data representing how to change the game tree to accomodate for a specific graft.
 */
typedef struct basilisk_specific_graft {
    /** Size, in bytes, of the graft's arguments. */
    unsigned long args_size;
    /** Pointer (can be null) to the graft's arguments data. This data is copied to the engine by
    functions that take the containing struct type. */
    void *args;

    /** Procedure called to add the graft. */
    basilisk_entity * (*graft_procedure)(basilisk_entity *entity, void *graft_args);
} basilisk_specific_graft;

/**
 * @brief Data representing the behavior to of an entity on receiving an event.
 */
typedef struct basilisk_specific_event_subscription {
    /** Relative priority (to other callbacks) of the callback when receiving an event. The higher, the later the callback will receive the event. */
    int index;
    /** Fucntion executed when an event is received. */
    void (*callback)(basilisk_entity *self_data, void *event_data);
} basilisk_specific_event_subscription;

/**
 * @brief Data representing a new event to add to the stack.
 */
typedef struct basilisk_specific_event {
    /** If set, the event will not be removed if the entity that sent it is removed itself. */
    bool is_detached;

    /** Size, in bytes, of the event's data.*/
    unsigned long data_size;
    /** The event's specific data. */
    void *data;
} basilisk_specific_event;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// ---- INTERFACE METHODS

// -------------------------------------------------------------------------------------------------
// LIFETIME FUNCTIONS

/* Creates an instance of the engine on the heap. */
basilisk_engine *basilisk_engine_create(void);
/* Destroys a previously allocated engine instance and nullifies the given pointer. */
void basilisk_engine_destroy(basilisk_engine **handle);

// -------------------------------------------------------------------------------------------------
// ENGINE METHODS

/* Returns the data bound to the root entity. This pointer has no memory behind it and should not be dereferenced. */
basilisk_entity *basilisk_engine_root_entity(basilisk_engine *handle);
/* Starts the main loop of the engine, resolving pending commands, sending events and stepping
entities. */
void basilisk_engine_run(basilisk_engine *handle, int fps);

// -------------------------------------------------------------------------------------------------
// ENTITY INTERACTIONS

/* Flags the engine to exit next frame. */
void basilisk_entity_quit(basilisk_entity *entity);
/* Adds a pending command to subscribe a callback to an event, by the event's name. */
void basilisk_entity_queue_subscribe_to_event(basilisk_entity *entity, const char *str_event_name, basilisk_specific_event_subscription subscription_data);
/* Sends an event to subscribed entities. */
void basilisk_entity_stack_event(basilisk_entity *entity, const char *str_event_name, basilisk_specific_event event_data);

// -------------------------------------------------------------------------------------------------
// ENTITY HIERARCHY MODIFICATIONS

/* Adds another entity to the game tree as a child of another. */
basilisk_entity *basilisk_entity_add_child(basilisk_entity *entity, const char *str_id, basilisk_specific_entity user_data);
/* Adds a pending command to remove an entity from the game tree. */
void basilisk_entity_queue_remove(basilisk_entity *entity);
/* Realizes a graft in the game tree relative to an entity. */
basilisk_entity *basilisk_entity_graft(basilisk_entity *entity, basilisk_specific_graft graft_data);

// -------------------------------------------------------------------------------------------------
// ENTITY SEARCHING

/* Search for a parent of a certain name and / or definition an entity might be related to. If the name and definition are NULL, the first parent is returned. */
basilisk_entity *basilisk_entity_get_parent(basilisk_entity *entity, const char *str_parent_name, const basilisk_entity_definition *entity_def);
/* Search for a child entity located at a specific path relative to an entity, while optionally checking if it was created with a specific definition. */
basilisk_entity *basilisk_entity_get_child(basilisk_entity *entity, const char *str_path, const basilisk_entity_definition *entity_def);
/* Resolves wether or not the entity has been defined using a specific definition or was marked as subtyping it. */
bool basilisk_entity_is(const basilisk_entity *entity, basilisk_entity_definition entity_def);


// -------------------------------------------------------------------------------------------------
// RESOURCE HANDLING

/* Requiered before using a resource. In nominal development mode (BASILISK_RELEASE is unset), this function will update the resource file from the filesystem to the storage file.
   Otherwise, this function will check that the resource is present in the given storage file and log what it finds. */
void basilisk_engine_declare_resource(basilisk_engine *handle, const char *str_storage_name, const char *str_file_path);
#define basilisk_engine_declare_resource(handle, str_storage_name, str_file_path) basilisk_engine_declare_resource(handle, BASILISK_RESOURCE_STORAGES_FOLDER "/" str_storage_name "." BASILISK_RESOURCE_STORAGES_EXTENSION, str_file_path)

/* Loads a storage file if not already and search for a resource in it, returning its data. */
void *basilisk_entity_fetch_resource(basilisk_entity *entity, const char *str_storage_name, const char *str_file_path, unsigned long *out_size);
#define basilisk_entity_fetch_resource(entity, str_storage_name, str_file_path, out_size) basilisk_entity_fetch_resource(entity, BASILISK_RESOURCE_STORAGES_FOLDER "/" str_storage_name "." BASILISK_RESOURCE_STORAGES_EXTENSION, str_file_path, out_size)

#endif
