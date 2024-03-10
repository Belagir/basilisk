
#include <SDL2/SDL.h>

#include <grafts/sdl2_window.h>

void init_bare_sdl(void *self_data, tarasque_engine *handle);
void deinit_bare_sdl(void *self_data, tarasque_engine *handle);

void init_bare_sdl(void *self_data, tarasque_engine *handle)
{
    (void) self_data;

    // add window & others
    SDL_Init(SDL_INIT_EVERYTHING);
}

void deinit_bare_sdl(void *self_data, tarasque_engine *handle)
{
    (void) self_data;

    if (SDL_WasInit(0)) {
        SDL_Quit();
    }
}

const entity_user_data graft_sdl2_window = {
        .data_size = 0u,
        .data = NULL,

        .on_init = &init_bare_sdl,
        .on_deinit = &deinit_bare_sdl,
        .on_frame = NULL,
};
