add_library(position position.cpp square.cpp move.hpp chess.cpp bitboard.cpp magic.cpp fen.cpp zobrist.cpp)
target_include_directories(position PUBLIC .)