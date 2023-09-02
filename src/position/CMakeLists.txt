add_library(position zobrist.cpp position.cpp square.cpp move.cpp chess.cpp bitboard.cpp magic.cpp fen.cpp transposition.cpp)
target_include_directories(position PUBLIC .)