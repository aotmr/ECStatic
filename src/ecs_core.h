#ifndef ECSTATIC_ECS_H
#define ECSTATIC_ECS_H

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>

#define ECS_NULL_ENTITY UINT32_MAX
#define ECS_MAX_COMPS 64
#define ECS_MAX_SYSTEMS 16

typedef struct ecs_comp_s ecs_comp;
typedef struct ecs_system_s ecs_system;
typedef struct ecs_world_s ecs_world;

struct ecs_comp_s {
    uint32_t size;

    void *user_data;

    void (*on_add)(ecs_world *world, uint32_t id, int ci, void *data);

    void (*on_remove)(ecs_world *world, uint32_t id, int ci, void *data);

    char *data;
};

struct ecs_system_s {
    uint64_t require, exclude;

//    void (*process)(ecs_world *world, uint32_t id);

    int (*compare)(ecs_world *world, uint32_t id1, uint32_t id2);

    uint32_t set_len;
    uint32_t *set;
};

struct ecs_world_s {
    uint32_t max;

    uint32_t free_top;
    uint32_t *free_list;

    uint64_t *masks;
    uint64_t dirty;

    uint8_t comps_num;
    uint8_t systems_num;

    ecs_comp comps[ECS_MAX_COMPS];
    ecs_system systems[ECS_MAX_SYSTEMS];
};

void ecs_init_world(ecs_world *world, uint32_t max);

void ecs_release_world(ecs_world *world);

int ecs_register_comp(ecs_world *world, const ecs_comp *comp);

int ecs_register_system(ecs_world *world, const ecs_system *system);

// Entity allocation

uint32_t ecs_create_entity(ecs_world *world);

void ecs_destroy_entity(ecs_world *world, uint32_t id);

// Component data

void *ecs_entity_add(ecs_world *world, uint32_t id, int ci);

void ecs_entity_remove(ecs_world *world, uint32_t id, int ci);

static inline void *ecs_entity_get(ecs_world *world, uint32_t id, int ci);

static inline bool ecs_entity_has(ecs_world *world, uint32_t id, int ci);

static inline bool ecs_entity_has_all(ecs_world *world, uint32_t id, uint64_t require, uint64_t exclude);

// System processing

void ecs_process_begin(ecs_world *world);

static inline void ecs_process_system(ecs_world *world, int si, void (*process)(ecs_world *world, uint32_t id));

void ecs_process_finish(ecs_world *world);

// Helper functions

void ecs_free_on_remove(ecs_world *world, uint32_t id, int ci, void *data);

// Inline functions

static inline void *ecs_entity_get(ecs_world *world, uint32_t id, int ci) {
    assert(world != NULL);
    assert(id < world->max);
    assert(0 <= ci && ci < ECS_MAX_COMPS);

    assert(ecs_entity_has(world, id, ci));

    struct ecs_comp_s *comp = &world->comps[ci];
    return comp->data + comp->size * id;
}

static inline bool ecs_entity_has(ecs_world *world, uint32_t id, int ci) {
    assert(world != NULL);
    assert(id < world->max);
    assert(0 <= ci && ci < ECS_MAX_COMPS);

    return (world->masks[id] & 1ULL << ci) != 0;
}

static inline bool ecs_entity_has_all(ecs_world *world, uint32_t id, uint64_t require, uint64_t exclude) {
    assert(world != NULL);
    assert(id < world->max);
    assert((require & exclude) == 0);

    uint64_t mask = world->masks[id];
    return (mask & require) == require;
//    return (mask & require) == require && ((mask & exclude) == 0);
}

static inline void ecs_process_system(ecs_world *world, int si, void (*process)(ecs_world *world, uint32_t id)) {
    assert(world != NULL);
    assert(0 <= si && si < ECS_MAX_SYSTEMS);

    ecs_system *system = &world->systems[si];

    for (uint32_t i = 0; i < system->set_len; ++i)
        process(world, system->set[i]);
}

#endif //ECSTATIC_ECS_H
