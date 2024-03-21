
#ifndef __STARSHIP_H__
#define __STARSHIP_H__

#include <SDL2/SDL.h>

#include <tarasque.h>

typedef struct starship {
    int x, y;

    SDL_Surface *sprite;
} starship;

tarasque_specific_entity starship_entity(starship *args);

#endif
