/**
 * @file base_entity_sdl_event_relay.c
 * @author gabriel ()
 * @brief Implementation file for a BE_event_relay_sdl entity.
 * @version 0.1
 * @date 2024-04-25
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <SDL2/SDL.h>

#include <base_entities/sdl_entities.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

#define BE_EVENT_RELAY_SDL_BUFFER_SIZE (64)     ///< Fixed size of the event buffer array. This is the number of events the event relay can re-send through the engine in one frame.

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/* Frame callback for a BE_event_relay_sdl entity. Polls new SDL Events and sends them back through the engine. */
static void BE_event_relay_sdl_on_frame(tarasque_entity *self_data, float elapsed_ms);

// -------------------------------------------------------------------------------------------------

/**
 * @brief Private data layout of an "sdl event relay" entity.
 *
 * @see BE_STATIC_event_relay_sdl, BE_DEF_event_relay_sdl
 */
typedef struct BE_event_relay_sdl {
    /** Internal buffer to re-order the polled events. Overriden each frame. */
    SDL_Event event_buffer[BE_EVENT_RELAY_SDL_BUFFER_SIZE];
} BE_event_relay_sdl;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Frame callback for a BE_event_relay_sdl entity.
 *
 * Polls events from the SDL library and resends them through the engine as an engine event.
 * SDL uses a FIFO collection to handle event, while the tarasque engine uses a FILO collection. On a frame,
 * the entity will poll the X first events, buffer them, and send them as events in the reverse order they were
 * received. This have the effect of stacking SDL events in the order SDL received them.
 *
 * @param[inout] self_data pointer to a BE_event_relay_sdl object
 * @param[in] elapsed_ms number of elapsed ms since last frame
 */
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
 * @brief Returns a NULL object, because the BE_event_relay_sdl entity does not need an external memory object to be initialized.
 * This function is provided for coherence with other entities and to future proof against possible extentions to this entity.
 *
 * @see BE_event_relay_sdl, BE_DEF_event_relay_sdl
 *
 * @return tarasque_entity *
 */
tarasque_entity *BE_STATIC_event_relay_sdl(void)
{
    return NULL;
}

// -------------------------------------------------------------------------------------------------

/**
 * @brief Defines the entity properties of a BE_event_relay_sdl entity
 *
 * The goal of this entity is to poll sdl events and retransmit them through the engine's event stack. It might be a child of a BE_sdl_context entity.
 * The events transfered are stacked in a way that reflects the order they were polled : the later the event is polled, the deeper it will be placed on the event stack.
 *
 * This entity might send two events : "sdl event" and "sdl event quit"
 *  - "sdl event" is associated to a pointer to a SDL_Event object. It is one of the SDL events the entity polled on last frame.
 *  - "sdl event quit" is not associated to any data. It just carries the information that the users wants to quit the window.
 *
 * @see BE_STATIC_event_relay_sdl, BE_event_relay_sdl
 *
 */
const tarasque_entity_definition BE_DEF_event_relay_sdl = {
        .data_size = sizeof(BE_event_relay_sdl),
        .on_frame = &BE_event_relay_sdl_on_frame,
};

