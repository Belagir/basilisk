
#include <grafts/sdl_window.h>

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
    tarasque_entity_graft(entity, "Window", graft_sdl_window(&(graft_sdl_window_args) {
            .for_window = {
                    .title = "shooty",
                    .x = SDL_WINDOWPOS_CENTERED, .y = SDL_WINDOWPOS_CENTERED,
                    .w = 1200, .h = 800,
            },
            .for_renderer = {
                    .clear_color = { 20u, 20u, 20u, 255u },
            } }));

    tarasque_entity_queue_subscribe_to_event(entity, "sdl event quit", (tarasque_specific_event_subscription) { .callback = &quit_game });

    tarasque_entity_add_child(tarasque_entity_get_child(entity, "Context/Window/Render Manager", NULL), "ship",
            starship_entity(&(starship) {
                    .sprite.body.local.position = { 10, 10 },
                    .sprite.body.local.scale = { 1, 1 },
                    .sprite.draw_index = 1,
            } ));

    tarasque_entity_add_child(tarasque_entity_get_child(entity, "Context/Window/Render Manager", NULL), "background", backround_entity(&(backround) { 0u }));
}

/**
 * @brief
 *
 * @return tarasque_entity_definition
 */
tarasque_specific_entity side_shooty_game(void)
{
    return (tarasque_specific_entity) {
            .entity_def = {
                    .on_init = &init,
            }
    };
}
