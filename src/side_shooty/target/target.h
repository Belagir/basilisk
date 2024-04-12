
#ifndef __TARGET_H__
#define __TARGET_H__

#include <SDL2/SDL.h>

#include <tarasque_bare.h>

#include <base_entities/sdl_entities.h>

typedef struct target {
    BE_shape_2D shape;
} target;

tarasque_specific_entity target_entity(target *args);

#endif
