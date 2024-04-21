
#include <ustd/math.h>

#include <base_entities/sdl_entities.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief Data layout of a "2D body" entity. Use it to configure 2D information of objects interacting in the world plane, or change the position of an object through its local position.
 */
typedef struct BE_body_2D BE_body_2D;
typedef struct BE_body_2D {
    /** Parent 2D body this body is pulling its global position from, automatically pulled from the entity's parents. Could be NULL. Overriden on initialization. */
    BE_body_2D *previous;

    /** Local position of the object in respect to its optional previous parent 2D body. */
    properties_2D local;
    /** Global position of the object on the world plane. Overriden on each frame. */
    properties_2D global;
} BE_body_2D;

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/*  */
static void BE_body_2D_init(tarasque_entity *self_data);

/*  */
static void BE_body_2D_on_frame(tarasque_entity *self_data, float elapsed_ms);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param self_data
 */
static void BE_body_2D_init(tarasque_entity *self_data)
{
    if (!self_data) {
        return;
    }

    BE_body_2D *self_body = (BE_body_2D *) self_data;

    self_body->previous = tarasque_entity_get_parent(self_data, NULL, &BE_DEF_body_2D);
}

/**
 * @brief
 *
 * @param self_data
 * @param elapsed_ms
 */
static void BE_body_2D_on_frame(tarasque_entity *self_data, float elapsed_ms)
{
    (void) elapsed_ms;

    if (!self_data) {
        return;
    }

    BE_body_2D *self_body = (BE_body_2D *) self_data;

    if (self_body->previous) {
        self_body->global.scale    = vector2_members_product(self_body->previous->global.scale, self_body->local.scale);
        self_body->global.position = vector2_members_product(self_body->local.scale, vector2_add(self_body->previous->global.position, self_body->local.position));
        self_body->global.angle    = fmodf(self_body->previous->global.angle + self_body->local.angle, PI_T_2);
    } else {
        self_body->global = self_body->local;
    }
}

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/**
 * @brief
 *
 * @param properties
 * @return tarasque_entity*
 */
tarasque_entity *BE_STATIC_body_2D(properties_2D properties)
{
    static BE_body_2D buffer = { 0u };

    buffer = (BE_body_2D) {
            .local = properties,
    };

    return &buffer;
}

/**
 * @brief
 *
 * @param body_2D
 * @param how
 * @return properties_2D
 */
properties_2D BE_body_2D_get(BE_body_2D *body_2D, BE_body_2D_space how)
{
    if (!body_2D) {
        return (properties_2D) { 0u };
    }

    switch (how) {
        case BODY_2D_SPACE_GLOBAL:
            return body_2D->global;
        case BODY_2D_SPACE_LOCAL:
            return body_2D->local;
        default:
            return (properties_2D) { 0u };
    }
}

/**
 * @brief
 *
 * @param body_2D
 * @param new_properties
 */
void BE_body_2D_set(BE_body_2D *body_2D, properties_2D new_properties)
{
    if (!body_2D) {
        return;
    }

    body_2D->local = new_properties;
}

/**
 * @brief
 *
 * @param body_2D
 * @param change
 */
void BE_body_2D_translate(BE_body_2D *body_2D, vector2_t change)
{
    if (!body_2D) {
        return;
    }

    body_2D->local.position = vector2_add(body_2D->local.position, change);
}

// -------------------------------------------------------------------------------------------------

/**
 * @brief
 */
const tarasque_entity_definition BE_DEF_body_2D = {
        .data_size = sizeof(BE_body_2D),
        .on_init = BE_body_2D_init,
        .on_frame = &BE_body_2D_on_frame,
};
