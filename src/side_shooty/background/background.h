
#ifndef __BACKGROUND_H__
#define __BACKGROUND_H__

#include <tarasque.h>
#include <SDL2/SDL.h>

typedef struct backround {
    SDL_Texture *texture;
} backround;

tarasque_specific_entity backround_entity(backround *bg);

#endif
