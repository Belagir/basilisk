
#include <stdio.h>

#include <tarasque.h>
#include <grafts/sdl2_window.h>

int main(void)
{
    tarasque_engine *handle = tarasque_engine_create();

    tarasque_entity_add_child(tarasque_engine_root_entity(handle), "", "Game", (entity_user_data) { 0u });

    tarasque_entity_graft(tarasque_engine_root_entity(handle), "Game", "SDL Graft", (graft_user_data) {
            .args = &(graft_sdl_window_args) { .title = "hello",  .x = SDL_WINDOWPOS_CENTERED, .y = SDL_WINDOWPOS_CENTERED, .w = 1200, .h = 800 },
            .args_size = GRAFT_SDL_WINDOW_ARGS_SIZE,
            .graft_procedure = &graft_sdl_window,
    });

    tarasque_engine_run(handle, 60);

    tarasque_engine_destroy(&handle);

    return 0;
}
