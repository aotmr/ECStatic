#ifndef ECSTATIC_UTILITY_H
#define ECSTATIC_UTILITY_H

#define ECS_EXPAND_INDEX(prefix, name, ...) prefix ## _ID_ ## name,

#define ECS_EXPAND_MASK(prefix, name, ...) prefix ## _MASK_ ## name = 1ULL << prefix ## _ID_ ## name,

#define ECS_EXPAND_REGISTER(world, name, type, ...) \
    ecs_register_comp(world, &(ecs_comp){ .size = sizeof(type), __VA_ARGS__ });

#define ECS_ENUM_INDEX(name, prefix, COMPS)\
    enum name { \
       COMPS(ECS_EXPAND_INDEX, prefix) \
       ECS_COMP_COUNT_ \
    }

#define ECS_ENUM_MASK(name, prefix, COMPS) \
    enum name { \
       COMPS(ECS_EXPAND_MASK, prefix) \
    }

#define ECS_REGISTER_COMPS(world, COMPS) \
    do { \
        COMPS(ECS_EXPAND_REGISTER, world) \
    } while(0)

#endif //ECSTATIC_UTILITY_H
