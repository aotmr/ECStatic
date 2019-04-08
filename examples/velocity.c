#include "ecs_core.h"
#include "ecs_utility.h"

#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>

// Component indices and masks

/* The _ argument is used internally for passing extra data through the table
 * (for example, the prefix of the enum members). */
#define COMPS(X, _) \
    X(_, EXISTS,    struct {}) \
    X(_, POS,       struct vec2) \
    X(_, VEL,       struct vec2)

struct vec2 {
    float x, y;
};

ECS_ENUM_INDEX(comp_index, COMP, COMPS);
ECS_ENUM_MASK(comp_mask, COMP, COMPS);

// System callbacks

static void process_move(ecs_world *world, uint32_t id) {
    struct vec2 *pos = ecs_entity_get(world, id, COMP_ID_POS);
    struct vec2 *vel = ecs_entity_get(world, id, COMP_ID_VEL);

    pos->x += vel->x;
    pos->y += vel->y;
}

static void process_debug(ecs_world *world, uint32_t id) {
    printf("%" PRIu32, (unsigned int) id);

    if (ecs_entity_has(world, id, COMP_ID_POS)) {
        struct vec2 *pos = ecs_entity_get(world, id, COMP_ID_POS);
        printf("\tpos (%4.1f, %4.1f)", pos->x, pos->y);
    }

    if (ecs_entity_has(world, id, COMP_ID_VEL)) {
        struct vec2 *vel = ecs_entity_get(world, id, COMP_ID_VEL);
        printf("\tvel (%4.1f, %4.1f)", vel->x, vel->y);
    }

    puts("");
}

int main() {
    ecs_world world;

    // World setup

    ecs_init_world(&world, 10);
    ECS_REGISTER_COMPS(&world, COMPS);

    // Register systems

    int system_move = ecs_register_system(&world, &(ecs_system) {
            .require = COMP_MASK_POS | COMP_MASK_VEL | COMP_MASK_EXISTS,
            .process = process_move,
    });

    int system_debug = ecs_register_system(&world, &(ecs_system) {
            .require = COMP_MASK_POS,
            .process = process_debug,
    });

    // Entity setup

    for (int i = 0; i < 10; ++i) {
        uint32_t id = ecs_create_entity(&world);

        // Every entity has a position
        struct vec2 *pos = ecs_entity_add(&world, id, COMP_ID_POS);
        pos->x = i;
        pos->y = 0;

        // Some have a velocity as well
        if (i % 3 == 0) {
            struct vec2 *vel = ecs_entity_add(&world, id, COMP_ID_VEL);
            vel->x = 1;
            vel->y = 2;
        }
    }

    // Simulation

    for (int i = 0; i < 10; ++i) {
        ecs_process_begin(&world);

        puts("------------------------------------------------------------------------");
        ecs_process_system(&world, system_move);
        ecs_process_system(&world, system_debug);

        ecs_process_finish(&world);
    }

    ecs_release_world(&world);
    return EXIT_SUCCESS;
}