add_compile_options(-Wall -Wextra -Wpedantic -O3)
set (CMAKE_CXX_STANDARD 20)
add_subdirectory(position)

add_executable(fce fce.cpp)
target_link_libraries(fce position)