add_library(core SHARED bitboard.cpp chess.cpp)
target_include_directories(core PUBLIC .)