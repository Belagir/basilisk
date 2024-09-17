
# Basilisk game engine -- a C system for barebone gamedev

This game engine is a low-dependency, highly modular game engine for C game development. It features a tree-oriented game structure, global events, and static resource managment, and an interface to easily add packages of entities to create a custom game environment !

The engine does not comes, strictly speaking, with a specific graphics backend directly integrated : the engine itself is only made to manage abstract data in a way that makes thinking about the structure of the game simple. Althrough for now, one "backend" is available as a package of entities (called a "graft"), a simple SDL2 rendering ensemble.

Using the engine looks like this :

```c
#include <stdio.h>
#include <basilisk.h>

static void hello(basilisk_entity *self_data) { puts("\nhello... "); }
static void bye(basilisk_entity *self_data) { puts(", bye !"); }
static void world(basilisk_entity *self_data) { puts("world"); }

int main(void)
{
    struct basilisk_engine *engine = basilisk_engine_create();

    // adds entity "hello" bellow the root
    struct basilisk_entity *first_entity = basilisk_entity_add_child(basilisk_engine_root_entity(engine), "hello",
            (struct basilisk_specific_entity) { { 0u, &hello, &bye, NULL }, NULL });

    // adds entity "world" bellow "hello" (accessed with "hello/world")
    (void) basilisk_entity_add_child(first_entity, "world",
            (struct basilisk_specific_entity) { { 0u, &world, &world, NULL }, NULL });

    // runs the engine until SIGINT
    basilisk_engine_run(engine, 60);

    basilisk_engine_destroy(&engine);

    return 0;
}
```

## Integration & build


