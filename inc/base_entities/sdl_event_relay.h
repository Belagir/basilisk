
#ifndef __BASE_ENTITY_SDL_EVENT_RELAY_H__
#define __BASE_ENTITY_SDL_EVENT_RELAY_H__

#include <tarasque.h>

#include <SDL2/SDL.h>

#define BASE_ENTITY_SDL_EVENT_RELAY_BUFFER_SIZE (64)

typedef struct be_sdl_event_relay_data {
    SDL_Event event_buffer[BASE_ENTITY_SDL_EVENT_RELAY_BUFFER_SIZE];
} be_sdl_event_relay_data;

tarasque_specific_entity be_sdl_event_relay(be_sdl_event_relay_data *base);

#endif
