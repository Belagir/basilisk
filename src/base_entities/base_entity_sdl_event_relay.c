
#include <SDL2/SDL.h>

#include <base_entities/sdl_entities.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

#define BE_EVENT_RELAY_SDL_BUFFER_SIZE (64)     ///< Fixed size of the event buffer array. This is the number of events the event relay can re-send through the engine in one frame.

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static void BE_event_relay_sdl_on_frame(tarasque_entity *self_data, float elapsed_ms);

// -------------------------------------------------------------------------------------------------

/**
 * @brief Data layout of an "event relay" base entity.
 */
typedef struct BE_event_relay_sdl {
    /** Internal buffer to re-order the polled events. Overriden each frame. */
    SDL_Event event_buffer[BE_EVENT_RELAY_SDL_BUFFER_SIZE];
} BE_event_relay_sdl;

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

/**
 * @brief
 *
 * @param
 * @return
 */
tarasque_entity *BE_STATIC_event_relay_sdl(void)
{
    return NULL;
}

// -------------------------------------------------------------------------------------------------

/**
 * @brief
 * This entity is made to be child of an sdl context (the BE_DEF_context_sdl entity) and will poll sdl events and retransmits them in the engine's event stack.
 * The events transfered are stacked in a way that reflects the order they were polled : the later the event is polled, the deeper it will be placed on the event stack.
 */
const tarasque_entity_definition BE_DEF_event_relay_sdl = {
        .data_size = sizeof(BE_event_relay_sdl),
        .on_frame = &BE_event_relay_sdl_on_frame,
};

