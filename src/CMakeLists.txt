add_subdirectory(game)

add_executable(fce fce.cpp)
target_link_libraries(fce game)