
#include <grafts/sdl2_window.h>

#include "graft_entities.h"

#include <stdio.h>

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

typedef struct graft_entity_sdl_render_manager_data {
    graft_entity_sdl_render_manager_args init;

    SDL_Renderer *renderer;
} graft_entity_sdl_render_manager_data;

static graft_entity_sdl_render_manager_data graft_entity_sdl_render_manager_data_buffer = { 0u };

// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static void graft_entity_sdl_render_manager_init(void *self_data, tarasque_entity_scene *scene);

static void graft_entity_sdl_render_manager_deinit(void *self_data, tarasque_entity_scene *scene);

static void graft_entity_sdl_render_manager_on_frame(void *self_data, float elapsed_ms, tarasque_entity_scene *scene);

static void graft_entity_sdl_render_manager_pre_draw(void *self_data, void *event_data, tarasque_entity_scene *scene);
static void graft_entity_sdl_render_manager_post_draw(void *self_data, void *event_data, tarasque_entity_scene *scene);
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

static void graft_entity_sdl_render_manager_init(void *self_data, tarasque_entity_scene *scene)
{
    (void) scene;

    if (!self_data) {
        return;
    }

    graft_entity_sdl_render_manager_data *init_data = (graft_entity_sdl_render_manager_data *) self_data;
    init_data->renderer = SDL_CreateRenderer(init_data->init.source_window, -1, init_data->init.flags);

    if (init_data->renderer) {
        tarasque_entity_scene_subscribe_to_event(scene, "sdl renderer pre draw", &graft_entity_sdl_render_manager_pre_draw);
        tarasque_entity_scene_subscribe_to_event(scene, "sdl renderer post draw", &graft_entity_sdl_render_manager_post_draw);
    }
}

static void graft_entity_sdl_render_manager_deinit(void *self_data, tarasque_entity_scene *scene)
{
    (void) scene;

    if (!self_data) {
        return;
    }

    graft_entity_sdl_render_manager_data *data = (graft_entity_sdl_render_manager_data *) self_data;

    SDL_DestroyRenderer(data->renderer);
    data->renderer = NULL;
}

static void graft_entity_sdl_render_manager_on_frame(void *self_data, float elapsed_ms, tarasque_entity_scene *scene)
{
    if (!self_data) {
        return;
    }

    graft_entity_sdl_render_manager_data *data = (graft_entity_sdl_render_manager_data *) self_data;

    printf("events\n");
    tarasque_entity_scene_stack_event(scene, "sdl renderer post draw", 0u, NULL, false);
    tarasque_entity_scene_stack_event(scene, "sdl renderer draw", sizeof(graft_sdl_window_event_draw), &(graft_sdl_window_event_draw) { data->renderer }, false);
    tarasque_entity_scene_stack_event(scene, "sdl renderer pre draw", 0u, NULL, false);
}

static void graft_entity_sdl_render_manager_pre_draw(void *self_data, void *event_data, tarasque_entity_scene *scene)
{
    (void) event_data;
    (void) scene;

    if (!self_data) {
        return;
    }

    printf("pre draw\n");

    graft_entity_sdl_render_manager_data *data = (graft_entity_sdl_render_manager_data *) self_data;

    SDL_SetRenderTarget(data->renderer, NULL);
    SDL_SetRenderDrawColor(data->renderer, 255u, 0u, 126u, 255u);
    SDL_RenderClear(data->renderer);
}

static void graft_entity_sdl_render_manager_post_draw(void *self_data, void *event_data, tarasque_entity_scene *scene)
{
    (void) event_data;
    (void) scene;

    if (!self_data) {
        return;
    }

    printf("post draw\n");

    graft_entity_sdl_render_manager_data *data = (graft_entity_sdl_render_manager_data *) self_data;

    SDL_RenderPresent(data->renderer);
}
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------------------

entity_user_data graft_entity_sdl_render_manager(graft_entity_sdl_render_manager_args args)
{
    // avoiding stack allocation but still won't force the user to deal with the heap...
    graft_entity_sdl_render_manager_data_buffer = (graft_entity_sdl_render_manager_data) {
            .init = args,
            .renderer = NULL,
    };

    return (entity_user_data) {
            .data_size = sizeof(graft_entity_sdl_render_manager_data_buffer),
            .data = &graft_entity_sdl_render_manager_data_buffer,

            .on_init = &graft_entity_sdl_render_manager_init,
            .on_frame = &graft_entity_sdl_render_manager_on_frame,
            .on_deinit = &graft_entity_sdl_render_manager_deinit,
    };
}
