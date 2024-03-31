
#ifndef __BACKGROUND_H__
#define __BACKGROUND_H__

#include <tarasque_bare.h>
#include <base_entities/sdl_entities.h>
#include <SDL2/SDL.h>

typedef struct backround {
    BE_texture_2D sprite;
} backround;

tarasque_specific_entity backround_entity(backround *bg);

#endif
