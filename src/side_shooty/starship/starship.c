
#include <ustd/res.h>

#include "starship.h"

// DECLARE_RES(ship_image, "res_ship_bmp")

extern const unsigned char _binary_res_ship_bmp_start[];
extern const unsigned char _binary_res_ship_bmp_end[];
extern const unsigned long _binary_res_ship_bmp_size;

static void init(tarasque_entity *entity)
{
    starship *ship = (starship *) entity;

    ship->sprite = SDL_CreateRGBSurfaceFrom((void *) _binary_res_ship_bmp_start, 64, 64, 32, 4, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
    printf("%#010x\n", ship->sprite);
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
