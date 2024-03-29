
#include <ustd/math.h>

#include <base_entities/sdl_entities.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

/*  */
static void BE_body_2D_on_frame(tarasque_entity *self_data, float elapsed_ms);

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

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
 */
const tarasque_entity_definition BE_body_2D_entity_def = {
        .data_size = sizeof(BE_body_2D),
        .on_frame = &BE_body_2D_on_frame,
};
