add_subdirectory(bitboard)

add_executable(fce fce.cpp)
target_link_libraries(fce bitboard)