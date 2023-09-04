add_library(core SHARED bitboard.cpp square.cpp chess.cpp)
target_include_directories(core PUBLIC .)