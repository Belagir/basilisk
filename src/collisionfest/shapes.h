
#ifndef __SHAPES_H__
#define __SHAPES_H__

#include <tarasque_bare.h>
#include <base_entities/sdl_entities.h>

struct shape {
    BE_shape_2D shape;
    bool is_controllable;
    SDL_Color color;
};

extern const tarasque_entity_definition shape_def;

#endif
