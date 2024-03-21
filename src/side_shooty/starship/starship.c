

#include <SDL2/SDL_image.h>

#include <ustd/res.h>

#include "starship.h"

extern const unsigned char _binary_res_ship_bmp_start[];
extern const unsigned char _binary_res_ship_bmp_end[];

static void init(tarasque_entity *entity)
{
    starship *ship = (starship *) entity;

    ship->sprite = IMG_Load_RW(SDL_RWFromConstMem(_binary_res_ship_bmp_start, (int) ((size_t) _binary_res_ship_bmp_end - (size_t) _binary_res_ship_bmp_start)), 0);

}

static void deinit(tarasque_entity *entity)
{
    starship *ship = (starship *) entity;

    SDL_FreeSurface(ship->sprite);
}

tarasque_specific_entity starship_entity(starship *args)
{
    return (tarasque_specific_entity) {
            .data_size = sizeof(*args),
            .data = args,
            .callbacks = {
                .on_init = &init,
                .on_deinit = &deinit,
            },
    };
}
