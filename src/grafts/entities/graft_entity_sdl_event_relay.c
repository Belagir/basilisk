
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
            tarasque_entity_stack_event(self_data, "sdl event quit", (tarasque_specific_event) { .is_detached = true });
        } else {
            tarasque_entity_stack_event(self_data, "sdl event", (tarasque_specific_event) { .is_detached = false, .data_size = sizeof(event), .data = &event, });
        }
    }
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

tarasque_specific_entity graft_entity_sdl_event_relay(void)
{
    return (tarasque_specific_entity) {
            .callbacks = {
                    .on_frame = &graft_entity_sdl_event_relay_on_frame,
            }
    };
}
