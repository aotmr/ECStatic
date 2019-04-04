#include "ecstatic.h"

#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>

// Component indices and masks

#define ECS_COMPS(C) \
    C(EXISTS, struct {}) \
    C(POS, struct vec2) \
    C(VEL, struct vec2)

#define ECS_EXPAND_INDEX(name, ...) ECS_COMP_ ## name,

#define ECS_EXPAND_MASK(name, ...) ECS_MASK_ ## name = 1ULL << ECS_COMP_ ## name,

enum ecs_comp_index {
    ECS_COMPS(ECS_EXPAND_INDEX)
};

enum ecs_comp_mask {
    ECS_COMPS(ECS_EXPAND_MASK)
};

// Component types

struct vec2 {
    float x, y;
};

// System callbacks

static void process_move(ecs_world *world, uint32_t id) {
    struct vec2 *pos = ecs_get(world, id, ECS_COMP_POS);
    struct vec2 *vel = ecs_get(world, id, ECS_COMP_VEL);

    pos->x += vel->x;
    pos->y += vel->y;
}

static void process_debug(ecs_world *world, uint32_t id) {
    printf("%" PRIu32, (unsigned int) id);

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

#define ECS_EXPAND_REG(name, type, ...) \
    ecs_register_comp(&world, &(ecs_comp){ .size = sizeof(type), __VA_ARGS__ });

int main() {
    ecs_world world;

    // World setup

    ecs_init_world(&world, 10);
    ECS_COMPS(ECS_EXPAND_REG)

    int system_move = ecs_register_system(&world, &(ecs_system) {
            .require = ECS_MASK_POS | ECS_MASK_VEL | ECS_MASK_EXISTS,
            .process = process_move,
    });

    int system_debug = ecs_register_system(&world, &(ecs_system) {
            .require = ECS_MASK_EXISTS,
            .process = process_debug,
    });

    // Entity setup

    for (int i = 0; i < 10; ++i) {
        uint32_t id = ecs_create(&world);

        // Every entity has a position
        struct vec2 *pos = ecs_add(&world, id, ECS_COMP_POS);
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