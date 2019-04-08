#ifndef ECSTATIC_ECS_UTILITY_H
#define ECSTATIC_ECS_UTILITY_H

#define ECS_MASK1(a)            (1ULL << (a))
#define ECS_MASK2(a, b)         (1ULL << (a) | 1ULL << (b))
#define ECS_MASK3(a, b, c)      (1ULL << (a) | 1ULL << (b) | 1ULL << (c))
#define ECS_MASK4(a, b, c, d)   (1ULL << (a) | 1ULL << (b) | 1ULL << (c) | 1ULL << (d))

#endif //ECSTATIC_ECS_UTILITY_H
