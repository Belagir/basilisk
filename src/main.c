
#include <stdio.h>

#include <tarasque.h>
#include <grafts/sdl2_window.h>

int main(void)
{
    tarasque_engine *handle = tarasque_engine_create();

    tarasque_entity_scene_add_entity(tarasque_engine_root_scene(handle), "", "Game", (entity_user_data) { 0u });
    tarasque_engine_graft(handle, "Game", "SDL Graft", (graft_user_data) {
            .args_size = GRAFT_SDL_WINDOW_ARGS_SIZE,
            .args = &(graft_sdl_window_args) {
                    .title = "hello world !",
                    .x = SDL_WINDOWPOS_CENTERED, .y = SDL_WINDOWPOS_CENTERED,
                    .w = 1200, .h = 1200,
             },
            .graft_procedure = graft_sdl_window,
    });
    tarasque_engine_run(handle, 60);

    tarasque_engine_destroy(&handle);

    return 0;
}
