/**
 * @file tarasque_impl.h
 * @author gabriel ()
 * @brief Complemetary package-level header supplying additional functions the library user does
 * not need to know about.
 * @version 0.1
 * @date 2024-03-06
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef __TARASQUE_IMPL_H__
#define __TARASQUE_IMPL_H__

#include <tarasque.h>

#include "../entity/entity.h"

typedef struct tarasque_entity_scene {
    tarasque_engine *handle;
    entity *current_entity;
} tarasque_entity_scene;

/* Sets the engine's current entity to link future operations back to it. */
tarasque_engine *tarasque_engine_for(tarasque_engine *handle, entity *current_entity);

#endif
