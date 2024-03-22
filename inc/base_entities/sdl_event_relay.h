
#ifndef __BASE_ENTITY_SDL_EVENT_RELAY_H__
#define __BASE_ENTITY_SDL_EVENT_RELAY_H__

#include <tarasque.h>

#include <SDL2/SDL.h>

#define BE_EVENT_RELAY_SDL_BUFFER_SIZE (64)

typedef struct be_event_relay_sdl {
    SDL_Event event_buffer[BE_EVENT_RELAY_SDL_BUFFER_SIZE];
} be_event_relay_sdl;

tarasque_specific_entity be_event_relay_sdl_entity(be_event_relay_sdl *base);

#endif
