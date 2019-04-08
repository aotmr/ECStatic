#ifndef ECSTATIC_ECS_UTILITY_H
#define ECSTATIC_ECS_UTILITY_H

#define ECS_EXPAND_INDEX(prefix, name, ...) prefix ## _ID_ ## name,

#define ECS_EXPAND_MASK(prefix, name, ...) prefix ## _MASK_ ## name = 1ULL << (unsigned int)prefix ## _ID_ ## name,

#define ECS_EXPAND_REGISTER(world, name, type, ...) \
    ecs_register_comp(world, &(ecs_comp){ .size = sizeof(type), __VA_ARGS__ });

/* Macros for applications that use a singly-defined set of components
 * throughout. Component indices and masks are defined as an enum, and it is
 * assumed that only this set of components is registered. */

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

/* TODO:
 * Macros for applications that use multiple sets of components registered at
 * different times. Component indices and masks are defined as variables. */

#endif //ECSTATIC_ECS_UTILITY_H
