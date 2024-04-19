
#include "shapes.h"

static void init(tarasque_entity *self_data);

static void on_sdl_event(tarasque_entity *self_data, void *event_data);

const tarasque_entity_definition shape_def = {
    .subtype = &BE_DEF_shape_2D,
    .data_size = sizeof(struct shape),
    .on_init = &init,
};

static void init(tarasque_entity *self_data)
{
    struct shape *shape = (struct shape *) self_data;

    tarasque_entity_add_child(self_data, "visual", (tarasque_specific_entity) {
            .entity_def = BE_DEF_shape_2D_visual,
            .data = &(BE_shape_2D_visual) { .color = shape->color, .draw_index = 1 },
    });
    tarasque_entity_add_child(self_data, "hitbox", (tarasque_specific_entity) {
            .entity_def = BE_DEF_shape_2D_collider,
            .data = &(BE_shape_2D_collider) { 0u },
    });

    if (shape->is_controllable) {
        tarasque_entity_queue_subscribe_to_event(self_data, "sdl event", (tarasque_specific_event_subscription) { .callback = &on_sdl_event });
    }
}

static void on_sdl_event(tarasque_entity *self_data, void *event_data)
{
    union SDL_Event *event = (union SDL_Event *) event_data;
    struct shape *shape = (struct shape *) self_data;

    if (event->type != SDL_KEYDOWN) {
        return;
    }

    shape->shape.body.local.position.x += (f32) (event->key.keysym.scancode == SDL_SCANCODE_RIGHT) * 2.f;
    shape->shape.body.local.position.x -= (f32) (event->key.keysym.scancode == SDL_SCANCODE_LEFT)  * 2.f;
    shape->shape.body.local.position.y += (f32) (event->key.keysym.scancode == SDL_SCANCODE_DOWN)  * 2.f;
    shape->shape.body.local.position.y -= (f32) (event->key.keysym.scancode == SDL_SCANCODE_UP)    * 2.f;

    shape->shape.body.local.angle += (f32) (event->key.keysym.scancode == SDL_SCANCODE_E) * 0.314f;
    shape->shape.body.local.angle += (f32) (event->key.keysym.scancode == SDL_SCANCODE_Q) * 0.314f;
}
