
#ifndef __STARSHIP_H__
#define __STARSHIP_H__

#include <SDL2/SDL.h>

#include <tarasque.h>

typedef struct starship {
    int x, y;
    int vel_x, vel_y;

    SDL_Texture *sprite;
} starship;

tarasque_specific_entity starship_entity(starship *args);

#endif
