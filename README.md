# ⚡ ECStatic

*A minimal, extensible ECS framework in C99*

## Features

- **Cache-friendly data storage** --
  component data is stored in contiguous arrays
- **Zero runtime dynamic allocations** --
  all necessary memory is allocated at initialization
- **Extensible design** --
  the core is small enough to understand,
  yet flexible enough to tailor to most applications

## Credits

- [**flecs**](https://github.com/SanderMertens/flecs) --
  inspiration for my project,
  a more feature-heavy entity component system
- [**sokol**](https://github.com/floooh/sokol) --
  entity ID allocator inspired by pool allocator in sokol_gfx
