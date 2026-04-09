add_subdirectory(core)

add_library(position position.cpp move.cpp magic.cpp fen.cpp hash.cpp engine.cpp)
target_link_libraries(position core)
target_include_directories(position PUBLIC .)
target_compile_options(position PRIVATE -O3)