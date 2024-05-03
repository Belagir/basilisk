
#ifndef __SHAPES_H__
#define __SHAPES_H__

#include <tarasque.h>
#include <base_entities/sdl_entities.h>

struct shape {
    shape_2D_id shape_kind;
    properties_2D properties;

    tarasque_entity *body;

    bool is_controllable;
    SDL_Color color;
};

extern const tarasque_entity_definition shape_def;

#endif
