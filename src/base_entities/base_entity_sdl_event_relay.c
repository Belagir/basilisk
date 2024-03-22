
#include <SDL2/SDL.h>

#include <base_entities/sdl_event_relay.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static void be_sdl_event_relay_on_frame(tarasque_entity *self_data, float elapsed_ms);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static void be_sdl_event_relay_on_frame(tarasque_entity *self_data, float elapsed_ms)
{
    (void) self_data;
    (void) elapsed_ms;

    be_sdl_event_relay_data *relay = (be_sdl_event_relay_data *) self_data;
    SDL_Event event = { 0u };

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            tarasque_entity_stack_event(self_data, "sdl event quit", (tarasque_specific_event) { .is_detached = true });
        } else {
            tarasque_entity_stack_event(self_data, "sdl event", (tarasque_specific_event) { .is_detached = false, .data_size = sizeof(event), .data = &event, });
        }
    }
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

tarasque_specific_entity be_sdl_event_relay(be_sdl_event_relay_data *base)
{
    return (tarasque_specific_entity) {
            .callbacks = {
                    .on_frame = &be_sdl_event_relay_on_frame,
            }
    };
}
