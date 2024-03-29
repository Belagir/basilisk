
#ifndef __STARSHIP_H__
#define __STARSHIP_H__

#include <SDL2/SDL.h>

#include <tarasque_bare.h>

#include <base_entities/sdl_entities.h>

typedef struct starship {
    BE_texture_2D sprite;

    int vel_x, vel_y;
    bool is_shooting;

    SDL_Texture *bullets_sprite;
} starship;

tarasque_specific_entity starship_entity(starship *args);

#endif
