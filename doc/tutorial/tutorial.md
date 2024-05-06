
# Building a side scrolling shooter with Tarasque

-- Explanation + Goals

Prerequisites :

```
gcc-13
make
git
```

-- Steps

-- End result + why it matters

## Creating a project

-- Explanation + Goals

-- Steps

Create a new folder for your project. It will contain all your sources and the engine's.

```bash
mkdir tarasque-side-shooter
```

```bash
git init -b main
git submodule add git@github.com:Belagir/untarasque.git
git submodule update --init --recursive
```

```bash
touch main.c
```

Once our files have been set up, let's see how we can compile and link everything. You can use your prefered toolchain.

-- End result + why it matters

```
.
+-- main.c
|
+-- ...                         // your prefered toolchain goes here
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

-- what was learnt

-- segway to next step

## Running the engine

-- Explanation + Goals

-- Steps

-- End result + why it matters

```c
// main.c

#include <tarasque.h> // if you used gcc -Itarasque/inc

int main(void)
{
    tarasque_engine *handle = tarasque_create_handle(); // create an engine instance

    tarasque_engine_run(handle, 60);                    // run the loop until ^C

    tarasque_engine_destroy(&handle);                   // release everything

    return 0;
}

```

-- what was learnt

-- segway to next step

## Extending the engine with a provided graft : spawning a window


