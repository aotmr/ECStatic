#include "ecstatic.h"

#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>

// Component IDs

enum ecs_comp_id {
    ECS_COMP_EXISTS,
    ECS_COMP_POS,
    ECS_COMP_VEL,
};

enum ecs_comp_mask {
    ECS_MASK_EXISTS = 1ULL << ECS_COMP_EXISTS,
    ECS_MASK_POS = 1ULL << ECS_COMP_POS,
    ECS_MASK_VEL = 1ULL << ECS_COMP_VEL,
};

// Component types

struct vec2 {
    float x, y;
};

// System callbacks

static void process_move(struct ecs_world * world, uint32_t id) {
    struct vec2 * pos = ecs_get(world, id, ECS_COMP_POS);
    struct vec2 * vel = ecs_get(world, id, ECS_COMP_VEL);

    pos->x += vel->x;
    pos->y += vel->y;
}

static void process_debug(struct ecs_world * world, uint32_t id) {
    printf("%" PRIu32, (unsigned int)id);

    if (ecs_has(world, id, ECS_COMP_POS)) {
        struct vec2 *pos = ecs_get(world, id, ECS_COMP_POS);
        printf("\tpos (%4.1f, %4.1f)", pos->x, pos->y);
    }

    if (ecs_has(world, id, ECS_COMP_VEL)) {
        struct vec2 *vel = ecs_get(world, id, ECS_COMP_VEL);
        printf("\tvel (%4.1f, %4.1f)", vel->x, vel->y);
    }

    puts("");
}


int main() {
    struct ecs_world world;

    // World setup

    ecs_init_world(&world, 10);

    ecs_register_comp(&world, &(struct ecs_comp){ .size = 0 });
    ecs_register_comp(&world, &(struct ecs_comp){ .size = sizeof(struct vec2) });
    ecs_register_comp(&world, &(struct ecs_comp){ .size = sizeof(struct vec2) });

    int system_move = ecs_register_system(&world, &(struct ecs_system){
        .require = ECS_MASK_POS | ECS_MASK_VEL | ECS_MASK_EXISTS,
        .process = process_move,
        });

    int system_debug = ecs_register_system(&world, &(struct ecs_system){
        .require = ECS_MASK_EXISTS,
        .process = process_debug,
    });

    // Entity setup

    for (int i = 0; i < 10; ++i) {
        uint32_t id = ecs_create(&world);

        // Every entity has a position
        struct vec2 * pos = ecs_add(&world, id, ECS_COMP_POS);
        pos->x = i;
        pos->y = 0;

        // Some have a velocity as well
        if (i % 3 == 0) {
            struct vec2 *vel = ecs_add(&world, id, ECS_COMP_VEL);
            vel->x = 1;
            vel->y = 2;
        }
    }

    // Simulation

    for (int i = 0; i < 10; ++i) {
        ecs_begin(&world);

        puts("------------------------------------------------------------------------");
        ecs_process(&world, system_move);
        ecs_process(&world, system_debug);

        ecs_finish(&world);
    }

    ecs_release_world(&world);
    return EXIT_SUCCESS;
}