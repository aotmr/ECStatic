add_executable(example-velocity velocity.c)
target_link_libraries(example-velocity ECStatic)

add_executable(example-sorted sorted.c)
target_link_libraries(example-sorted ECStatic)