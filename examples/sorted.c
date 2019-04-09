#include "ecs_core.h"
#include "ecs_utility.h"

#include <string.h>
#include <stdio.h>

int g_comp_tag;

static int compare_tag(ecs_world *world, uint32_t id1, uint32_t id2) {
    char * tag1 = *(void **)ecs_entity_get(world, id1, g_comp_tag);
    char * tag2 = *(void **)ecs_entity_get(world, id2, g_comp_tag);

    printf("%s\t<=>\t%s\n", tag1, tag2);

    return strcmp(tag1, tag2);
}

static void process_print_tag(ecs_world *world, uint32_t id) {
    char * tag = *(void **)ecs_entity_get(world, id, g_comp_tag);
    printf("%d:\t%s\t\n", id, tag);
}

int main() {
    ecs_world world;
    ecs_init_world(&world, 10);

    g_comp_tag = ecs_register_comp(&world, &(ecs_comp){.size = sizeof(void *), .on_remove = ecs_free_on_remove});

    int s_tag = ecs_register_system(&world, &(ecs_system){.require = ECS_MASK1(g_comp_tag), .compare = compare_tag});

    static const char * tags[5] = {
            "red",
            "blue",
            "yellow",
            "green",
            "violet"
    };
    for (int i = 0; i < 5; ++i) {
        uint32_t id = ecs_create_entity(&world);
        char ** tag = ecs_entity_get(&world, id, g_comp_tag);
        *tag = strdup(tags[i]);
    }

    ecs_process_begin(&world);

    puts("------------------------------------------------------------------------");
    ecs_process_system(&world, s_tag, process_print_tag);

    ecs_process_finish(&world);

    ecs_release_world(&world);
}