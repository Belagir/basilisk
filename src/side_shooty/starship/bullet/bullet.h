
#ifndef __BULLET_H__
#define __BULLET_H__

#include <SDL2/SDL.h>
#include <tarasque_bare.h>

#include <base_entities/sdl_entities.h>

typedef struct bullet {
    BE_texture_2D sprite;
} bullet;

tarasque_specific_entity bullet_entity(bullet *args);

#endif
