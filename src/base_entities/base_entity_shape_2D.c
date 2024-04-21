
#include <base_entities/sdl_entities.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 */
typedef struct BE_shape_2D {
    BE_body_2D *body;

    shape_2D_id kind;
    union { shape_2D_circle as_circle; shape_2D_rect as_rect; };
} BE_shape_2D;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static void BE_shape_2D_init(tarasque_entity *self_data);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static void BE_shape_2D_init(tarasque_entity *self_data)
{
    BE_shape_2D *shape = (BE_shape_2D *) self_data;

    if (!shape) {
        return;
    }

    shape->body = tarasque_entity_get_parent(self_data, NULL, &BE_DEF_body_2D);
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param circle
 * @return
 */
tarasque_entity *BE_STATIC_shape_2D_circle(shape_2D_circle circle)
{
    static BE_shape_2D buffer = { 0u };

    buffer = (BE_shape_2D) {
            .kind = SHAPE_2D_CIRCLE,
            .as_circle = circle,
    };

    return &buffer;
}

/**
 * @brief
 *
 * @param rect
 * @return
 */
tarasque_entity *BE_STATIC_shape_2D_rectangle(shape_2D_rect rect)
{
    static BE_shape_2D buffer = { 0u };

    buffer = (BE_shape_2D) {
            .kind = SHAPE_2D_RECT,
            .as_rect = rect,
    };

    return &buffer;
}

/**
 * @brief
 *
 * @param shape_data
 * @return
 */
shape_2D_id BE_shape_2D_what(BE_shape_2D *shape_data)
{
    BE_shape_2D *shape = (BE_shape_2D *) shape_data;

    if (!shape) {
        return -1;
    }

    return shape->kind;
}

/**
 * @brief
 *
 * @param shape_data
 * @return
 */
shape_2D_circle *BE_shape_2D_as_circle(BE_shape_2D *shape)
{
    if (!shape || (shape->kind != SHAPE_2D_CIRCLE)) {
        return NULL;
    }

    return &(shape->as_circle);
}

/**
 * @brief
 *
 * @param shape_data
 * @return
 */
shape_2D_rect *BE_shape_2D_as_rect(BE_shape_2D *shape)
{
    if (!shape || (shape->kind != SHAPE_2D_RECT)) {
        return NULL;
    }

    return &(shape->as_rect);
}

/**
 * @brief
 *
 * @param shape
 * @return
 */
BE_body_2D *BE_shape_2D_get_body(BE_shape_2D *shape)
{
    if (!shape) {
        return NULL;
    }

    return shape->body;
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

const tarasque_entity_definition BE_DEF_shape_2D = {
        .data_size = sizeof(BE_shape_2D),
        .on_init = &BE_shape_2D_init,
};
