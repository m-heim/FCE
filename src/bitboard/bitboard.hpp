#include <cstdint>

typedef uint64_t Bitboard;

constexpr Bitboard fileABB = 0x0101010101010101;
constexpr Bitboard fileBBB = fileABB << 1;
constexpr Bitboard fileCBB = fileBBB << 1;
constexpr Bitboard fileDBB = fileCBB << 1;
constexpr Bitboard fileEBB = fileDBB << 1;
constexpr Bitboard fileFBB = fileEBB << 1;
constexpr Bitboard fileGBB = fileFBB << 1;
constexpr Bitboard fileHBB = fileGBB << 1;

constexpr Bitboard rank1BB = 0xFF;
constexpr Bitboard rank2BB = rank1BB * 8;
constexpr Bitboard rank3BB = rank2BB * 8;
constexpr Bitboard rank4BB = rank3BB * 8;
constexpr Bitboard rank5BB = rank4BB * 8;
