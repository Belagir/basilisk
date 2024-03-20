
#include "graft_entities.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static void graft_entity_sdl_event_relay_on_frame(tarasque_entity *self_data, float elapsed_ms);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static void graft_entity_sdl_event_relay_on_frame(tarasque_entity *self_data, float elapsed_ms)
{
    (void) self_data;
    (void) elapsed_ms;

    SDL_Event event = { 0u };

    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            tarasque_entity_stack_event(self_data, "sdl event quit", 0u, NULL, true);
        } else {
            tarasque_entity_stack_event(self_data, "sdl event", sizeof(event), &event, false);
        }
    }
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

tarasque_entity_specific_data graft_entity_sdl_event_relay(void)
{
    return (tarasque_entity_specific_data) {
            .callbacks = {
                    .on_frame = &graft_entity_sdl_event_relay_on_frame,
            }
    };
}
