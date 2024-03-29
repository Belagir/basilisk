
#include <SDL2/SDL.h>

#include <base_entities/sdl_entities.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static void BE_event_relay_sdl_on_frame(tarasque_entity *self_data, float elapsed_ms);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static void BE_event_relay_sdl_on_frame(tarasque_entity *self_data, float elapsed_ms)
{
    (void) elapsed_ms;

    BE_event_relay_sdl *relay = (BE_event_relay_sdl *) self_data;
    SDL_Event event = { 0u };
    size_t buffer_pos = 0u;

    while ((buffer_pos < BE_EVENT_RELAY_SDL_BUFFER_SIZE) && SDL_PollEvent(&event)) {
        relay->event_buffer[buffer_pos] = event;
        buffer_pos += 1u;
    }

    for ( ; buffer_pos > 0 ; buffer_pos--) {
        if (relay->event_buffer[buffer_pos - 1].type == SDL_QUIT) {
            tarasque_entity_stack_event(self_data, "sdl event quit", (tarasque_specific_event) { .is_detached = true });
        } else {
            tarasque_entity_stack_event(self_data, "sdl event", (tarasque_specific_event) { .is_detached = false, .data_size = sizeof(*relay->event_buffer), .data = relay->event_buffer + buffer_pos - 1, });
        }
    }
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

const tarasque_entity_definition BE_event_relay_sdl_entity_def = {
        .data_size = sizeof(BE_event_relay_sdl),
        .on_frame = &BE_event_relay_sdl_on_frame,
};

