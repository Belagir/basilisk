
#include <grafts/sdl_window.h>

#include "app.h"
#include "shapes.h"

static void init(tarasque_entity *self_data);
static void on_window_quit(tarasque_entity *self_data, void *event_data);

const tarasque_entity_definition application_def = {
        .on_init = &init,
};

static void init(tarasque_entity *self_data)
{
    tarasque_entity_graft(self_data, "window", graft_sdl_window(&(graft_sdl_window_args) {
            .for_window = { .w = 600, .h = 300, .title = "collisions", .x = SDL_WINDOWPOS_CENTERED, .y = SDL_WINDOWPOS_CENTERED }
    }));
    tarasque_entity_queue_subscribe_to_event(self_data, "sdl event quit", (tarasque_specific_event_subscription) { .callback = &on_window_quit });

    tarasque_entity_add_child(tarasque_entity_get_child(self_data, "Context/Window/Render Manager/Collision Manager", NULL),"shape 1", (tarasque_specific_entity) {
            .entity_def = shape_def,
            .data = &(struct shape) {
                    .is_controllable = true,
                    .color = { .r = 255, .a = 255 },
                    .properties = { .scale = { 1, 1 }, .position = { 100, 100 }, },
            }
    });

    tarasque_entity_add_child(tarasque_entity_get_child(self_data, "Context/Window/Render Manager/Collision Manager", NULL),"shape 2", (tarasque_specific_entity) {
            .entity_def = shape_def,
            .data = &(struct shape) {
                    .is_controllable = false,
                    .color = { .g = 255, .a = 255 },
                    .properties = { .scale = { 1, 1 }, .position = { 110, 200 }, },
            }
    });
}

static void on_window_quit(tarasque_entity *self_data, void *event_data)
{
    tarasque_entity_quit(self_data);
}
