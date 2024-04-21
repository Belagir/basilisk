
#include "shapes.h"

static void init(tarasque_entity *self_data);

static void on_sdl_event(tarasque_entity *self_data, void *event_data);

const tarasque_entity_definition shape_def = {
    .data_size = sizeof(struct shape),
    .on_init = &init,
};

static void init(tarasque_entity *self_data)
{
    struct shape *shape = (struct shape *) self_data;

    shape->body = tarasque_entity_add_child(self_data, "position", (tarasque_specific_entity) {
            .entity_def = BE_DEF_body_2D,
            .data = BE_STATIC_body_2D(shape->properties),
    });

    tarasque_entity *shape_impl = tarasque_entity_add_child(shape->body, "shape", (tarasque_specific_entity) {
            .entity_def = BE_DEF_shape_2D,
            .data = BE_STATIC_shape_2D_circle((shape_2D_circle) { .radius = 20.f }),
    });

    tarasque_entity_add_child(shape_impl, "visual", (tarasque_specific_entity) {
            .entity_def = BE_DEF_shape_2D_visual,
            .data = BE_STATIC_shape_2D_visual(shape->color, 1),
    });

    tarasque_entity_add_child(shape_impl, "hitbox", (tarasque_specific_entity) {
            .entity_def = BE_DEF_shape_2D_collider,
            .data = BE_STATIC_shape_2D_collider(0x0, 0x0),
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

    BE_body_2D_translate(shape->body, (vector2_t) {
            .x = (f32) ((event->key.keysym.scancode == SDL_SCANCODE_RIGHT) - (event->key.keysym.scancode == SDL_SCANCODE_LEFT)) * 2.f,
            .y = (f32) ((event->key.keysym.scancode == SDL_SCANCODE_DOWN)  - (event->key.keysym.scancode == SDL_SCANCODE_UP))   * 2.f,
    });
}
