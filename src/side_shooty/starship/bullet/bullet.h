
#ifndef __BULLET_H__
#define __BULLET_H__

#include <SDL2/SDL.h>
#include <tarasque_bare.h>

typedef struct bullet {
    int x, y;

    SDL_Texture *sprite;
} bullet;

tarasque_specific_entity bullet_entity(bullet *args);

#endif
