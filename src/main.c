
#include <stdio.h>

#include <tarasque.h>
#include <grafts/sdl2_window.h>

static void dummy_draw_rect(tarasque_entity *self_data, void *event_data)
{
    (void) self_data;

    graft_sdl_window_event_draw *draw_data = (graft_sdl_window_event_draw *) event_data;

    SDL_SetRenderDrawColor(draw_data->renderer, 0, 50u, 255u, 255u);
    SDL_RenderDrawRect(draw_data->renderer, &(const SDL_Rect) { 30, 30, 300, 400 });
}

static void dummy_draw_init(tarasque_entity *self_data)
{
    tarasque_entity_subscribe_to_event(self_data, "sdl renderer draw", (tarasque_event_subscription_specific_data) { .callback = &dummy_draw_rect, });
}

int main(void)
{
    tarasque_engine *handle = tarasque_engine_create();

    tarasque_entity_add_child(tarasque_engine_root_entity(handle), "", "Game", (tarasque_entity_specific_data) { 0u });

    tarasque_entity_graft(tarasque_engine_root_entity(handle), "Game", "SDL Graft", (tarasque_graft_specific_data) {
            .args = &(graft_sdl_window_args) {
                    .for_window = {
                            .title = "hello",
                            .x = SDL_WINDOWPOS_CENTERED, .y = SDL_WINDOWPOS_CENTERED,
                            .w = 1200, .h = 800,
                    },
                    .for_renderer = {
                        .clear_color = (SDL_Color) { 30u, 30u, 30u, 255u },
                    }
            },
            .args_size = GRAFT_SDL_WINDOW_ARGS_SIZE,
            .graft_procedure = &graft_sdl_window,
    });

    tarasque_entity_add_child(tarasque_engine_root_entity(handle), "Game", "square", (tarasque_entity_specific_data) {
            .callbacks = {
                    .on_init = &dummy_draw_init
            },
    });

    tarasque_engine_run(handle, 60);

    tarasque_engine_destroy(&handle);

    return 0;
}
