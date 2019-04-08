#include "ecs_core.h"

#include <string.h>

void ecs_init_world(ecs_world *world, uint32_t max) {
    assert(world != NULL);
    assert(max < UINT32_MAX);

    memset(world, 0, sizeof(ecs_world));

    world->max = max;

    world->free_list = calloc(max, sizeof(uint32_t));
    for (uint32_t i = 0; i < max; ++i)
        world->free_list[i] = i;

    world->masks = calloc(max, sizeof(uint64_t));
}

void ecs_release_world(ecs_world *world) {
    assert(world != NULL);

    for (uint32_t id = 0; id < world->max; ++id) {
        if (ecs_entity_has(world, id, 0))
            ecs_entity_remove(world, id, 0);
    }
    free(world->free_list);
    free(world->masks);

    for (int i = 0; i < world->comps_num; ++i)
        free(world->comps[i].data);

//    for (int i = 0; i < world->systems_num; ++i)
//        free(world->systems[i].set);

    memset(world, 0, sizeof(ecs_world));
}

int ecs_register_comp(ecs_world *world, const ecs_comp *comp) {
    assert(world != NULL);
    assert(comp != NULL);

    assert(world->comps_num < ECS_MAX_COMPS);
    assert(comp->data == NULL);

    int ci = world->comps_num++;
    ecs_comp *comp2 = &world->comps[ci];
    *comp2 = *comp;
    comp2->data = calloc(world->max, comp2->size);
    return ci;
}

int ecs_register_system(ecs_world *world, const ecs_system *system) {
    assert(world != NULL);
    assert(system != NULL);

    assert(world->systems_num < ECS_MAX_SYSTEMS);
    assert((system->require & system->exclude) == 0);
//    assert(system->set_len == 0);
//    assert(system->set == NULL);

    int si = world->systems_num++;
    ecs_system *system2 = &world->systems[si];
    *system2 = *system;
//    system2->set = calloc(world->max, sizeof(uint32_t));
//    system2->set_len = 0;

    return si;
}

// Entity allocation

uint32_t ecs_create_entity(ecs_world *world) {
    assert(world != NULL);

    if (world->free_top >= world->max)
        return ECS_NULL_ENTITY;
    else {
        uint32_t id = world->free_list[world->free_top++];
        ecs_entity_add(world, id, 0);
        return id;
    }
}

void ecs_destroy_entity(ecs_world *world, uint32_t id) {
    assert(world != NULL);
    assert(id < world->max);

    for (int i = 0; i < 64; ++i)
        ecs_entity_remove(world, id, i);
}

// Component data

void *ecs_entity_add(ecs_world *world, uint32_t id, int ci) {
    assert(world != NULL);
    assert(id < world->max);
    assert(0 <= ci && ci < ECS_MAX_COMPS);

    assert(!ecs_entity_has(world, id, ci));

    ecs_comp *comp = &world->comps[ci];

    world->masks[id] |= 1ULL << ci;
    world->dirty |= 1ULL << ci;

    void *data = comp->data + comp->size * id;
    if (comp->on_add)
        comp->on_add(world, id, ci, data);
    return data;
}

void ecs_entity_remove(ecs_world *world, uint32_t id, int ci) {
    assert(world != NULL);
    assert(id < world->max);
    assert(0 <= ci && ci < ECS_MAX_COMPS);

    assert(ecs_entity_has(world, id, ci));

    ecs_comp *comp = &world->comps[ci];

    void *data = comp->data + comp->size * id;
    if (comp->on_remove)
        comp->on_remove(world, id, ci, data);

    world->masks[id] &= ~(1ULL << ci);
    world->dirty |= 1ULL << ci;
}

// System processing

void ecs_process_begin(ecs_world *world) {

}

void ecs_process_system(ecs_world *world, int si) {
    assert(world != NULL);
    assert(0 <= si && si < ECS_MAX_SYSTEMS);

    ecs_system *system = &world->systems[si];
    assert(system->compare == NULL);

    for (uint32_t id = 0; id < world->max; ++id) {
        if (ecs_entity_has_all(world, id, system->require, system->exclude))
            system->process(world, id);
    }
}

void ecs_process_finish(ecs_world *world) {
    world->dirty = 0;
}

// Helper functions

void ecs_free_on_remove(ecs_world *world, uint32_t id, int ci, void *data) {
    free(data);
}
