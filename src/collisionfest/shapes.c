
#include "shapes.h"

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static void init(tarasque_entity *self_data);

static void on_sdl_event(tarasque_entity *self_data, void *event_data);

static void on_collision(tarasque_entity *entity, BE_shape_2D_collider *hit, BE_shape_2D_collider *other);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

const tarasque_entity_definition shape_def = {
    .data_size = sizeof(struct shape),
    .on_init = &init,
};

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static void init(tarasque_entity *self_data)
{
    struct shape *shape = (struct shape *) self_data;
    tarasque_entity *new_shape_details = NULL;

    shape->body = tarasque_entity_add_child(self_data, "position", (tarasque_specific_entity) {
            .entity_def = BE_DEF_body_2D,
            .data = BE_STATIC_body_2D(shape->properties),
    });

    switch (shape->shape_kind) {
        case SHAPE_2D_CIRCLE:
            new_shape_details = BE_STATIC_shape_2D_circle((shape_2D_circle) { .radius = 20.f });
            break;
        case SHAPE_2D_RECT:
            new_shape_details = BE_STATIC_shape_2D_rectangle((shape_2D_rect) { .height = 30.f, .width = 50.f });

    }
    BE_shape_2D *shape_impl = tarasque_entity_add_child(shape->body, "shape", (tarasque_specific_entity) {
            .entity_def = BE_DEF_shape_2D,
            .data = new_shape_details,
    });

    tarasque_entity_add_child(shape_impl, "visual", (tarasque_specific_entity) {
            .entity_def = BE_DEF_shape_2D_visual,
            .data = BE_STATIC_shape_2D_visual(shape->color, 1),
    });

    BE_shape_2D_collider *hitbox = tarasque_entity_add_child(shape_impl, "hitbox", (tarasque_specific_entity) {
            .entity_def = BE_DEF_shape_2D_collider,
            .data = BE_STATIC_shape_2D_collider(0x0, 0x0),
    });

    BE_shape_2D_collider_set_callback(hitbox, SHAPE_2D_COLLIDER_SITUATION_IS_INSIDE, (BE_shape_2D_collider_callback_info) {
            .subject = self_data,
            .callback = &on_collision,
    });

    if (shape->is_controllable) {
        tarasque_entity_queue_subscribe_to_event(self_data, "sdl event", (tarasque_specific_event_subscription) { .callback = &on_sdl_event });
    }
}

// -------------------------------------------------------------------------------------------------

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

// -------------------------------------------------------------------------------------------------

static void on_collision(tarasque_entity *entity, BE_shape_2D_collider *hit, BE_shape_2D_collider *other)
{
    struct shape *shape = (struct shape *) entity;

    BE_body_2D_translate(shape->body, vector2_scale(0.0005f, vector2_substract(
            BE_body_2D_get(shape->body, BODY_2D_SPACE_GLOBAL).position,
            BE_body_2D_get(BE_shape_2D_collider_get_body(other), BODY_2D_SPACE_GLOBAL).position
    )));
}