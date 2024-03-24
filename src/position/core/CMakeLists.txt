add_library(core SHARED bitboard.cpp chess.cpp logger.cpp util.cpp)
target_include_directories(core PUBLIC .)