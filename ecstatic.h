#ifndef ECSTATIC_ECS_H
#define ECSTATIC_ECS_H

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#define ECS_NULL_ENTITY UINT32_MAX
#define ECS_MAX_COMPS 64
#define ECS_MAX_SYSTEMS 16

struct ecs_world;

struct ecs_comp {
    uint32_t size;

    void *user_data;

    void (*on_add)(struct ecs_world *world, uint32_t id, int ci, void *data);
    void (*on_remove)(struct ecs_world *world, uint32_t id, int ci, void *data);

    char *data;
};

struct ecs_system {
    uint64_t require, exclude;

    void (*process)(struct ecs_world *world, uint32_t id);
    int (*compare)(struct ecs_world *world, uint32_t id1, uint32_t id2);

//    uint32_t set_len;
//    uint32_t *set;
};

struct ecs_world {
    uint32_t max;

    uint32_t free_top;
    uint32_t *free_list;

    uint64_t *masks;
    uint64_t dirty;

    uint8_t comps_num;
    uint8_t systems_num;

    struct ecs_comp comps[ECS_MAX_COMPS];
    struct ecs_system systems[ECS_MAX_SYSTEMS];
};

void ecs_init_world(struct ecs_world *world, uint32_t max);

void ecs_release_world(struct ecs_world *world);

int ecs_register_comp(struct ecs_world *world, const struct ecs_comp *comp);

int ecs_register_system(struct ecs_world *world, const struct ecs_system *system);

// Entity allocation

uint32_t ecs_create(struct ecs_world *world);

void ecs_destroy(struct ecs_world *world, uint32_t id);

// Component data

void *ecs_add(struct ecs_world *world, uint32_t id, int ci);

void ecs_remove(struct ecs_world *world, uint32_t id, int ci);

static inline void *ecs_get(struct ecs_world *world, uint32_t id, int ci);

static inline bool ecs_has(struct ecs_world *world, uint32_t id, int ci);

static inline bool ecs_match(struct ecs_world *world, uint32_t id, uint64_t require, uint64_t exclude);

// System processing

void ecs_begin(struct ecs_world *world);

void ecs_process(struct ecs_world *world, int si);

void ecs_finish(struct ecs_world *world);

// Helper functions

void ecs_free_on_remove(struct ecs_world *world, uint32_t id, int ci, void *data);

// Inline functions

static inline void *ecs_get(struct ecs_world *world, uint32_t id, int ci) {
    assert(world != NULL);
    assert(id < world->max);
    assert(0 <= ci && ci < ECS_MAX_COMPS);

    assert(ecs_has(world, id, ci));

    struct ecs_comp *comp = &world->comps[ci];
    return comp->data + comp->size * id;
}

static inline bool ecs_has(struct ecs_world *world, uint32_t id, int ci) {
    assert(world != NULL);
    assert(id < world->max);
    assert(0 <= ci && ci < ECS_MAX_COMPS);

    return (world->masks[id] & 1ULL << ci) != 0;
}

static inline bool ecs_match(struct ecs_world *world, uint32_t id, uint64_t require, uint64_t exclude) {
    assert(world != NULL);
    assert(id < world->max);
    assert((require & exclude) == 0);

    uint64_t mask = world->masks[id];
    return (mask & require) == require && ((mask & exclude) == 0);
}

#endif //ECSTATIC_ECS_H
