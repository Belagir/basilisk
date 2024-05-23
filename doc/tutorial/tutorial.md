
# Building a side scrolling shooter with Tarasque

-- Explanation + Goals

Prerequisites :

```
gcc-13
ar
make
git
libSDL2
```

Knowing how to setup a C project.

-- Steps

-- End result + why it matters

## Creating a project

Our first step is to create our project. Since the tarasque engine does not impose a specific toolchain or external tools to actually work, you have a lot of liberty on how you will organize your project. Liberty is nice when you know what you are doing. If you are confident in the intricacies of C project management, feel free to cook your own file structure & toolchain and adapt the commands shown in this section.

### File organization

Create a new folder for your project. It will contain all your sources and the engine's.

```bash
mkdir tarasque-side-shooter && cd tarasque-side-shooter
```

Next, let's bring the library in scope :

```bash
git init -b main
git submodule add git@github.com:Belagir/untarasque.git
git submodule update --init --recursive
```

Now that we have the files, let's build our library for the architecture of your computer :

```bash
make -C untarasque
```

Once the library successfully compiled and archived, you should have access to two interesting files : `untarasque/bin/libtarasque.a` and `untarasque/inc/tarasque.h`. Those two be very useful for our next steps.

For now, let's start our own game with a main file and our own cozy source directory :

```bash
mkdir src
touch src/main.c
```
And you can fill our new main file with the good old `main()` function for now.

### Compilation & linking

Once our files have been set up, let's see how we can compile and link everything. You can use your prefered toolchain for that, but be sure to include those options to have access to the tarasque library :

 - compilation flags : `-Iuntarasque/inc`
 - linker flags : `-Luntarasque/bin -ltarasque -lm`

> Notice the  `-lm` flag going *after* the tarasque library flag.

And for the purpose of this turorial :

 - compilation flags : `-std=c2x -lm`
 - linker flags : `` `sdl2-config --cflags --libs` `` (with the backticks !)

Right now, you should be able to compile the project to an empty executable, and your file structure should look something like that :

```
.
+-- src
    +-- main.c
|
+-- ...                         // your prefered toolchain here ?
|
+-- tarasque
    |
    +-- bin
    |   +-- libtarasque.a       // <---- link this in your project !
    |
    +-- build
    |   +-- ...                 // object files
    |
    +-- inc                     // <---- header folder, include this in your project !
        +-- tarasque.h
        +-- ...                 // other useful headers in their dedicated folders
    |
    +-- ...                     // tarasque source files
    |
    +-- unstandard              // support library must be present (use $git submodule update)
    |
    +-- Makefile
```

If our project compiles, that means we can dive in the functional side of the game !

## Running the engine

The first thing we want to do is create an engine instance. This is done through the `tarasque_engine` opaque type and `tarasque_engine_*()` namespace, that will change the global state of the program. We will not interact a lot with such operations though (there are not many methods tied to this type anyway) but we need to instanciate one to control the lifetime of our game engine :

```c
// main.c

#include <tarasque.h>                                   // if you used gcc -Itarasque/inc

int main(void)
{
    tarasque_engine *handle = tarasque_engine_create(); // create an engine instance

    tarasque_engine_run(handle, 60);                    // run the loop until ^C

    tarasque_engine_destroy(&handle);                   // release everything

    return 0;
}

```

`tarasque_engine_create()` builds an engine instance on the heap that will need to be released whith `tarasque_engine_destroy()`. We pass a pointer to the latter so it can be set to `NULL` for us.

The interesting part of our program is `tarasque_engine_run()`. This methods blocks on a loop until an interupt is sent (or by another way that is not important right now).

The engine itself has three main responsabilities :

 - store and release abstract, user defined **game entities** in a **game tree** ;
 - broadcast abstract, user defined **game events** to entities that subscribed to it ;
 - load, unload and give access to filesystem **resources** for entities.

By itself, the engine brings only basic functionalities. Game entities are meant to extend the possibilities, but developping everything from the ground up seems sub-ideal. This is why the tarasque engine gives basic entities to work with, and even organize pre-planned entities subtrees called **grafts**.

We will use one graft in particular : the `graft_sdl_window`. It will provide use with the necessary utilities to create a 2D game in our own OS window.

## Extending the engine with a provided graft : spawning a window

Local changes to the game state needs to go through the special `tarasque_entity` anonymous type and the `tarasque_entity_*()` namespace. To access the tree from an engine handle, we need to use `tarasque_engine_root_entity()`. This function returns the empty root entity of our tree, and we will be able to build from here.

The `tarasque_entity_*()` namespace is way more populated than the `tarasque_engine_*()` one, and most of our actions to create a game will be using those. Right now, we need to look at `tarasque_entity_graft()` :

```c
void tarasque_entity_graft(tarasque_entity *entity, tarasque_specific_graft graft_data);
```

This function will take a game entity `entity` (so, for us, the root) and graft a set of entities `graft_data` below it, as children. This information is stored in the `tarasque_specific_graft` structure but we don't need to unravel its contents to use it. We just need to use it in conjunction with the right utility method present in a `"grafts/*.h"` header to get a result.

> Every `tarasque_specific_*` type is meant to be used with a function like the one we just saw. Those data structures are here to provide a clear and well delimited interface to operations needing a lot of parameters that might not be relevant in totality or that are tedious to write again and again and again.

Using this function to add a "SDL window" graft below our root would look like this :

```c
// main.c

#include <tarasque.h>
#include <grafts/sdl_window.h>                          // bringing the graft to scope

int main(void)
{
    tarasque_engine *handle = tarasque_engine_create();

    tarasque_entity_graft(tarasque_engine_root_entity(handle),              // adding the graft to the game tree
            graft_sdl_window(&(graft_sdl_window_args) {
                    .for_window = {
                            .x = SDL_WINDOWPOS_CENTERED, .y = SDL_WINDOWPOS_CENTERED,
                            .w = 1200, .h = 600,
                            .title = "side shooty",
                    },
    }));

    tarasque_engine_run(handle, 60);

    tarasque_engine_destroy(&handle);

    return 0;
}

```

Notice how the graft is not added through a structure directly, but with a function returning such a structure.

From what we can gather from the graft's documentation (or the console output), our game tree now looks like this :

```
      ""
      |
   "Context"
      |
   "Window"
   /       \
"Event    "Render
 Relay"    Manager"
             |
          "Collision
           Manager"
```

Now that some entities have been added to our game tree, we should examine **what does it means to the engine to have an entity as child to another** ?

> In the engine, **an entity is considered to depend on all its parents to work**. The engine will assume that if an entity is removed all children entities are obsolete and are to also be removed.
On the opposite direction, **an entity is extended by all its children** and its behavior is changed and complexified by all entities below.

For our new game tree, we have a SDL manager (`"Context"`) that is extended to actually display a window (by `"Window"`), render stuff on it (by `"Render Manager"`), and listen for user events through SDL and send them back through the engine (by `"Event Relay"`). Our rendering entity is further extended to allow collision between objects (with `"Collision Manager"`).

Including the graft header also brings the main SDL2 header and a flutter of basic entities. We will use those to create our little game.

## The encapsulating game entity

For our tranquility of mind, we will create an entity that, when added to the game tree, will carry our whole game below it. Create a new folder named "game" and the files "game/game.h" and "game/game.c" :


```
.
+--src
|   +-- main.c
|   |
|   +-- game
|       +-- game.h
|       +-- game.c
|
+-- ...
|
+-- tarasque
```

So we can demonstrate how we can create a fully functional entity while exposing the strict minimum in the header file.

The end goal is to use the `tarasque_entity_add_child()` function :

```c
tarasque_entity *tarasque_entity_add_child(tarasque_entity *entity, const char *str_id, tarasque_specific_entity user_data);
```

This function will create a new entity specified by `user_data` with the name `str_id`, and add it below another `entity`. The function returns a pointer to the data of the new entity.

This time, we will need to take a look to the members of the `tarasque_specific_entity` structure (and `tarasque_entity_definition` substructure), because we will create our own custom entity :

```c
typedef struct tarasque_entity_definition {
    unsigned long data_size;

    void (*on_init)(tarasque_entity *self_data);
    void (*on_deinit)(tarasque_entity *self_data);
    void (*on_frame)(tarasque_entity *self_data, float elapsed_ms);
} tarasque_entity_definition;

typedef struct tarasque_specific_entity {
    tarasque_entity_definition entity_def;
    tarasque_entity *data;
} tarasque_specific_entity;
```

### How an entity works

The `tarasque_entity_definition` structure describes what the engine needs to know about our entity to interact with it : we may see it as some kind of dynamic class. It holds information about how many bytes the engine needs to allocate for it, a callback to run at the creation of the entity (`.on_init`) the data inside the entity, a callback to run when the entity is removed (`.on_deinit`), and a callback to call on each frame of the main loop (`.on_frame`).

Once this is defined, we pass data in the `tarasque_specific_entity` structure that matches in length the `.data_size` field in the definition passed as `.entity_def` (or a NULL pointer if we don't care about the entity's starting state -- in this case, the contents of the entity will be all zeroes).

### The game entity

```c
// game/game.h

#ifndef __GAME_H__
#define __GAME_H__

#include <tarasque.h>

extern const tarasque_specific_entity game_entity;

#endif

```

```c
// game/game.c

#include "game.h"

#include <stddef.h>

const tarasque_specific_entity game_entity = {
        .entity_def = {
                .data_size = 0u,

                .on_init   = NULL,
                .on_deinit = NULL,
                .on_frame  = NULL,
        },
        .data = NULL,
};

```

```c
// main.c

// ...

#include "game/game.h"

int main(void)
{
    // ...

    tarasque_entity_add_child(tarasque_entity_get_child(tarasque_engine_root_entity(handle), "Context/Window/Render Manager/Collision Manager", NULL), "game", game_entity);

    tarasque_engine_run(handle, 60);

    // ...
}

```

```c
// game/game.c

// ...

static void game_init(tarasque_entity *self_data);

static void game_init(tarasque_entity *self_data)
{
    // game creation will go here
}

const tarasque_specific_entity game_entity = {
        .entity_def = {
                .data_size = 0u,

                .on_init   = &game_init,
                .on_deinit = NULL,
                .on_frame  = NULL,
        },
        .data = NULL,
};

```

### Organizing and using game resources

create the resources directory :

```
.
+-- res
|
+-- src
|   +-- ...
|
+-- ...                         // your prefered toolchain here ?
|
+-- tarasque
|   +-- ...

```

This directory will hold our game resources such as images, sounds, etc. Basicaly, everything that must be packaged with the game for it to run, but isn't code.
For now, we want to populate it with our background, starship, bullet, and enemy sprites :

![almost decent background]()

![crude sprite of a starship]()

![crude sprite of a bullet]()

![crude sprite of an alien vessel]()

Just copy those sprites in the new `./res/` directory for now. To package those files with the game, we need to declare them to the engine :

```c
// main.c

// ...

#include "game/game.h"

int main(void)
{
    tarasque_engine *handle = tarasque_engine_create();

    tarasque_engine_declare_resource(handle, "default", "res/background.png");
    tarasque_engine_declare_resource(handle, "default", "res/starship.png");
    tarasque_engine_declare_resource(handle, "default", "res/bullet.png");
    tarasque_engine_declare_resource(handle, "default", "res/alien.png");

    // ...

    tarasque_engine_run(handle, 60);

    // ...
}

```

Once the game is compiled and tested, you will notice that the *executable*, when ran, will create a folder `./program_data/` in which a new `default.data` file lives. This file is updated everytime the engine encounters a resource declaration, and contains all of the resources declared as part  of the `"default"` resource storage.

This behavior is very inefficient and may induce stutter every time you launch your game. Fortunately, this only happens as long as you compile without the `TARASQUE_RELEASE` switch : if it were set, the game would just trust that the `default.data` file would exist and merely check that the declared resource exists in it. But as long as we are developping our game, we will live with it.

### Basic background texture


