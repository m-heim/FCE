add_compile_options(-Wall -Wextra -Wpedantic)
add_subdirectory(position)

add_executable(fce fce.cpp)
target_link_libraries(fce position)