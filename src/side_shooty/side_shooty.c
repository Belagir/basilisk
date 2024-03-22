
#include <grafts/sdl2_window.h>

#include "side_shooty.h"
#include "starship/starship.h"
#include "background/background.h"

/**
 * @brief
 *
 * @param entity
 * @param event_data
 */
static void quit_game(tarasque_entity *entity, void *event_data)
{
    (void) event_data;
    tarasque_entity_quit(entity);
}

/**
 * @brief
 *
 * @param entity
 */
static void init(tarasque_entity *entity)
{
    tarasque_entity_graft(entity, "", "Window", (tarasque_specific_graft) {
            .args = &(graft_sdl_window_args) {
                    .for_window = {
                            .title = "shooty",
                            .x = SDL_WINDOWPOS_CENTERED, .y = SDL_WINDOWPOS_CENTERED,
                            .w = 1200, .h = 800,
                    },
                    .for_renderer = {
                            .clear_color = { 20u, 20u, 20u, 255u },
                    }
            },
            .args_size = GRAFT_SDL_WINDOW_ARGS_SIZE,
            .graft_procedure = &graft_sdl_window
    });

    tarasque_entity_subscribe_to_event(entity, "sdl event quit", (tarasque_specific_event_subscription) { .callback = &quit_game });

    tarasque_entity_add_child(entity, "SDL Context/SDL Window/SDL Render Manager", "ship", starship_entity(&(starship) { .x = 10, .y = 10 }));
    tarasque_entity_add_child(entity, "SDL Context/SDL Window/SDL Render Manager", "background", backround_entity(&(backround) { 0u }));
}

/**
 * @brief
 *
 * @return tarasque_specific_entity
 */
tarasque_specific_entity side_shooty_game(void)
{
    return (tarasque_specific_entity) {
            .callbacks = {
                    .on_init = &init,
            }
    };
}
