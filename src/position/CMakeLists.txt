add_subdirectory(core)

add_library(position zobrist.cpp position.cpp move.cpp magic.cpp fen.cpp transposition.cpp)
target_link_libraries(position core)
target_include_directories(position PUBLIC .)