#include "ecs_core.h"
#include "ecs_utility.h"

#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>

// Component indices and masks

int g_comp_exists;
int g_comp_pos;
int g_comp_vel;

struct vec2 {
    float x, y;
};

// System callbacks

static void process_move(ecs_world *world, uint32_t id) {
    struct vec2 *pos = ecs_entity_get(world, id, g_comp_pos);
    struct vec2 *vel = ecs_entity_get(world, id, g_comp_vel);

    pos->x += vel->x;
    pos->y += vel->y;
}

static void process_debug(ecs_world *world, uint32_t id) {
    printf("%" PRIu32, (unsigned int) id);

    if (ecs_entity_has(world, id, g_comp_pos)) {
        struct vec2 *pos = ecs_entity_get(world, id, g_comp_pos);
        printf("\tpos (%4.1f, %4.1f)", pos->x, pos->y);
    }

    if (ecs_entity_has(world, id, g_comp_vel)) {
        struct vec2 *vel = ecs_entity_get(world, id, g_comp_vel);
        printf("\tvel (%4.1f, %4.1f)", vel->x, vel->y);
    }

    puts("");
}

int main() {
    ecs_world world;

    // World setup

    ecs_init_world(&world, 10);

    g_comp_exists = ecs_register_comp(&world, &(ecs_comp) {});
    g_comp_pos = ecs_register_comp(&world, &(ecs_comp) {.size = sizeof(struct vec2)});
    g_comp_vel = ecs_register_comp(&world, &(ecs_comp) {.size = sizeof(struct vec2)});

    // Register systems

    int system_move = ecs_register_system(&world, &(ecs_system) {
            .require = ECS_MASK3(g_comp_pos, g_comp_vel, g_comp_exists),
//            .process = process_move,
    });

    int system_debug = ecs_register_system(&world, &(ecs_system) {
            .require = ECS_MASK1(g_comp_exists),
//            .process = process_debug,
    });

    // Entity setup

    for (int i = 0; i < 10; ++i) {
        uint32_t id = ecs_create_entity(&world);

        // Every entity has a position
        struct vec2 *pos = ecs_entity_add(&world, id, g_comp_pos);
        pos->x = i;
        pos->y = 0;

        // Some have a velocity as well
        if (i % 3 == 0) {
            struct vec2 *vel = ecs_entity_add(&world, id, g_comp_vel);
            vel->x = 1;
            vel->y = 2;
        }
    }

    // Simulation

    for (int i = 0; i < 10; ++i) {
        ecs_process_begin(&world);

        puts("------------------------------------------------------------------------");
        ecs_process_system(&world, system_move, process_move);
        ecs_process_system(&world, system_debug, process_debug);

        ecs_process_finish(&world);
    }

    ecs_release_world(&world);
    return EXIT_SUCCESS;
}