add_compile_options(-Wall -Wextra -Wpedantic -Ofast)
set (CMAKE_CXX_STANDARD 20)
add_subdirectory(position)
add_subdirectory(magics)
target_link_libraries(magics position)

add_executable(fce fce.cpp)
target_link_libraries(fce position)