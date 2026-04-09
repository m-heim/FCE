#include "position.hpp"
#include "core.hpp"
#include "hash.hpp"
#include "move.hpp"
#include <bitboard.hpp>
#include <chess.hpp>
#include <cstring>
#include <iostream>
#include <square.hpp>

void Position::setSquare(SquareIndex squareVal, Color colorVal, Piece pieceVal) {
    Piece oldPiece = board[squareVal].piece;
    Color oldColor = board[squareVal].color;
    board[squareVal].color = colorVal;
    board[squareVal].piece = pieceVal;

    Bitboard mask = maskedSquare[squareVal];
    Bitboard unmasked = unmaskedSquare[squareVal];
    bitboards[oldColor][oldPiece] &= unmasked; // remove piece from bitboard
    occupation[oldColor] &= unmasked;          // remove piece from occupations

    bitboards[colorVal][pieceVal] |= mask; // add piece to bitboard
    occupation[colorVal] |= mask;          // add piece to occupations
}

bool Position::kingExists() {
    return bitboards[playerColor][Piece::KING] != 0;
}

std::string Position::stringify_board() {
    std::string ret;
    ret.append("  A B C D E F G H\n");
    for (int8_t row = Rank::RANK_TOP; row >= Rank::RANK_1; row--) {
        ret.push_back((char)('1' + row));
        for (int8_t col = File::FILE_A; col <= File::FILE_TOP; col++) {
            ret.push_back(' ');
            SquareIndex square = (row * Square::SQUARE_A2) + col;
            char piece = pieceToChar(board[square].piece);
            if (board[square].color == Color::WHITE && piece != ' ') {
                piece += 'A' - 'a';
            }
            ret.push_back(piece);
        }
        ret.push_back('\n');
    }
    return ret;
}

void Position::print_board() {
    std::cout << stringify_board() << std::endl;
}

Evaluation Position::evaluate() {
    Evaluation eval = 0;
    eval += evaluateMaterial();
    eval += evaluatePosition();
    return playerColor == Color::WHITE ? eval : -eval;
}

Evaluation Position::evaluateMaterial() {
    // NOTE Assuming king is top
    Evaluation eval = EvaluationLiterals::EVEN;
    for (uint8_t piece = 0; piece <= Piece::PIECE_TOP_VALID; piece++) {
        eval += (bitboardGetHW(bitboards[Color::WHITE][piece]) -
                 bitboardGetHW(bitboards[Color::BLACK][piece])) *
                evaluations[piece];
    }
    return eval;
}

Evaluation Position::evaluatePosition(void) {
    Evaluation eval = 0;
    eval += bitboardGetHW(bitboards[Color::WHITE][Piece::PAWN] & extendedCenter) *
            EvaluationLiterals::CENTER_PAWN_VAL;
    eval -= bitboardGetHW(bitboards[Color::BLACK][Piece::PAWN] & extendedCenter) *
            EvaluationLiterals::CENTER_PAWN_VAL;
    eval += std::max<int>(0, bitboardGetHW(bitboards[Color::WHITE][Piece::BISHOP]) - 1) *
            EvaluationLiterals::MULTIPLE_BISHOP_VAL;
    eval -= std::max<int>(0, bitboardGetHW(bitboards[Color::BLACK][Piece::BISHOP]) - 1) *
            EvaluationLiterals::MULTIPLE_BISHOP_VAL;
    for (int file = File::FILE_A; file <= File::FILE_H; file++) {
        eval +=
            std::max<Evaluation>(
                0, bitboardGetHW(bitboards[Color::WHITE][Piece::PAWN] & (fileAttacks[file])) - 1) *
            EvaluationLiterals::MULTIPLE_PAWN_VAL;
        eval -=
            std::max<Evaluation>(
                0, bitboardGetHW(bitboards[Color::BLACK][Piece::PAWN] & (fileAttacks[file])) - 1) *
            EvaluationLiterals::MULTIPLE_PAWN_VAL;
    }
    for (auto piece : {Piece::KNIGHT, Piece::BISHOP}) {
        eval += bitboardGetHW(bitboards[Color::WHITE][piece] & extendedCenter) *
                EvaluationLiterals::CENTER_PIECE_VAL;
        eval -= bitboardGetHW(bitboards[Color::BLACK][piece] & extendedCenter) *
                EvaluationLiterals::CENTER_PIECE_VAL;
    }
    eval += bitboardGetHW(bitboards[Color::WHITE][Piece::KING] &
                          (rank1 & (fileA | fileB | fileG | fileH))) *
            EvaluationLiterals::SAFE_KING_VAL;
    eval -= bitboardGetHW(bitboards[Color::BLACK][Piece::KING] &
                          (rank8 & (fileA | fileB | fileG | fileH))) *
            EvaluationLiterals::SAFE_KING_VAL;
    return eval;
}

Position::Position(TranspositionTable *tt, Zobrist *z) : transpositionTable(tt), zobrist(z) {
    SquareInfo empty = SquareInfo(Color::NO_COLOR, Piece::NO_PIECE);
    for (SquareIndex square = Square::SQUARE_A1; square <= Square::SQUARE_H8; square++) {
        board[square] = empty;
    }
    memset(&bitboards, 0, sizeof(bitboards));
    occupation[Color::WHITE] = 0ULL;
    occupation[Color::BLACK] = 0ULL;
    occupation[Color::NO_COLOR] = 0ULL;
    plies = 0;
    plies_since_capture = 0;
    playerColor = Color::WHITE;
    opponentPlayerColor = Color::BLACK;
    this->en_passant = SquareIndex::Squares::SQUARE_NONE;
}

static TranspositionTable &getDefaultTT() {
    static TranspositionTable tt(TRANSPOSITION_TABLE_N_PRESET);
    return tt;
}

static Zobrist &getDefaultZobrist() {
    static Zobrist z;
    return z;
}

Position::Position() : Position(&getDefaultTT(), &getDefaultZobrist()) {
}

Bitboard Position::computeHash() {
    Bitboard result = emptyBitboard;
    for (SquareIndex square = Square::SQUARE_A1; square <= Square::SQUARE_H8; square++) {
        SquareInfo squareInfo = board[square];
        if (squareInfo.piece != Piece::NO_PIECE) {
            result ^= zobrist->zobristKeys[squareInfo.color][squareInfo.piece][square];
        }
    }
    if (playerColor == Color::BLACK) {
        result ^= zobrist->zobristSide;
    }
    if (en_passant != Square::SQUARE_NONE) {
        uint8_t file = en_passant % Square::SQUARE_A2;
        result ^= zobrist->zobristEnPassant[file];
    }
    result ^=
        zobrist->zobristCastle[Color::WHITE][Castle::KINGSIDE]
                              [static_cast<int>(castle_rights[Color::WHITE][Castle::KINGSIDE])];
    result ^=
        zobrist->zobristCastle[Color::WHITE][Castle::QUEENSIDE]
                              [static_cast<int>(castle_rights[Color::WHITE][Castle::QUEENSIDE])];
    result ^=
        zobrist->zobristCastle[Color::BLACK][Castle::KINGSIDE]
                              [static_cast<int>(castle_rights[Color::BLACK][Castle::KINGSIDE])];
    result ^=
        zobrist->zobristCastle[Color::BLACK][Castle::QUEENSIDE]
                              [static_cast<int>(castle_rights[Color::BLACK][Castle::QUEENSIDE])];
    return result;
}

void Position::makeMove(Move m, UndoInfo &undoInfo) {
    SquareIndex from = m.getFrom();
    SquareIndex to = m.getTo();
    uint8_t flags = m.getFlags();
    SquareInfo movingPiece = board[from];
    SquareInfo leavingPiece = board[to];
    undoInfo.move = m;
    undoInfo.hash = hash;
    undoInfo.en_passant = en_passant;
    undoInfo.plies_since_capture = plies_since_capture;
    undoInfo.previousPiece = leavingPiece.piece;
    undoInfo.castle_rights[Color::WHITE][Castle::KINGSIDE] =
        castle_rights[Color::WHITE][Castle::KINGSIDE];
    undoInfo.castle_rights[Color::WHITE][Castle::QUEENSIDE] =
        castle_rights[Color::WHITE][Castle::QUEENSIDE];
    undoInfo.castle_rights[Color::BLACK][Castle::KINGSIDE] =
        castle_rights[Color::BLACK][Castle::KINGSIDE];
    undoInfo.castle_rights[Color::BLACK][Castle::QUEENSIDE] =
        castle_rights[Color::BLACK][Castle::QUEENSIDE];
    if (en_passant != Square::SQUARE_NONE) {
        uint8_t file = en_passant % 8;
        hash ^= zobrist->zobristEnPassant[file];
        en_passant = Square::SQUARE_NONE;
    }
    // while castling is allowed and the king doesnt move, keep this
    if (flags != MoveFlags::NULL_MOVE) {
        if (movingPiece.piece == Piece::KING) {
            if (castle_rights[playerColor][Castle::QUEENSIDE]) {
                castle_rights[playerColor][Castle::QUEENSIDE] = false;
                hash ^= zobrist->zobristCastle[playerColor][Castle::QUEENSIDE][true];
            }
            if (castle_rights[playerColor][Castle::KINGSIDE]) {
                castle_rights[playerColor][Castle::KINGSIDE] = false;
                hash ^= zobrist->zobristCastle[playerColor][Castle::KINGSIDE][true];
            }
        } else if (movingPiece.piece == Piece::ROOK) {
            SquareIndex queenside =
                Square::SQUARE_A1 + ((bool)playerColor * (int)Square::SQUARE_A8);
            SquareIndex kingside = Square::SQUARE_H1 + ((bool)playerColor * (int)Square::SQUARE_A8);
            if (from == queenside && castle_rights[playerColor][Castle::QUEENSIDE]) {
                castle_rights[playerColor][Castle::QUEENSIDE] = false;
                hash ^= zobrist->zobristCastle[playerColor][Castle::QUEENSIDE][true];
            } else if (from == kingside && castle_rights[playerColor][Castle::KINGSIDE]) {
                castle_rights[playerColor][Castle::KINGSIDE] = false;
                hash ^= zobrist->zobristCastle[playerColor][Castle::KINGSIDE][true];
            }
        } else if (leavingPiece.piece == Piece::ROOK) {
            SquareIndex queenside =
                Square::SQUARE_A1 + ((bool)opponentPlayerColor * (int)Square::SQUARE_A8);
            SquareIndex kingside =
                Square::SQUARE_H1 + ((bool)opponentPlayerColor * (int)Square::SQUARE_A8);
            if (to == queenside && castle_rights[opponentPlayerColor][Castle::QUEENSIDE]) {
                castle_rights[opponentPlayerColor][Castle::QUEENSIDE] = false;
                hash ^= zobrist->zobristCastle[opponentPlayerColor][Castle::QUEENSIDE][true];
            } else if (to == kingside && castle_rights[opponentPlayerColor][Castle::KINGSIDE]) {
                castle_rights[opponentPlayerColor][Castle::KINGSIDE] = false;
                hash ^= zobrist->zobristCastle[opponentPlayerColor][Castle::KINGSIDE][true];
            }
        }
    }
    if (flags == MoveFlags::QUIET) {
        setSquare(to, movingPiece.color, movingPiece.piece);
        setSquare(from, Color::NO_COLOR, Piece::NO_PIECE);
        hash ^= zobrist->zobristKeys[playerColor][movingPiece.piece][from];
        hash ^= zobrist->zobristKeys[playerColor][movingPiece.piece][to];
        plies_since_capture += 1;
    } else if (flags == MoveFlags::DOUBLE_PUSH) {
        setSquare(to, movingPiece.color, movingPiece.piece);
        setSquare(from, Color::NO_COLOR, Piece::NO_PIECE);
        hash ^= zobrist->zobristKeys[playerColor][movingPiece.piece][from];
        hash ^= zobrist->zobristKeys[playerColor][movingPiece.piece][to];
        en_passant = (from + to) / 2;
        hash ^= zobrist->zobristEnPassant[en_passant % 8];
        plies_since_capture += 1;
    } else if (flags == MoveFlags::CAPTURE) {

        setSquare(to, movingPiece.color, movingPiece.piece);
        setSquare(from, Color::NO_COLOR, Piece::NO_PIECE);
        hash ^= zobrist->zobristKeys[playerColor][movingPiece.piece][from];
        hash ^= zobrist->zobristKeys[playerColor][movingPiece.piece][to];
        hash ^= zobrist->zobristKeys[opponentPlayerColor][leavingPiece.piece][to];
        plies_since_capture = 0;
    } else if (flags == MoveFlags::CASTLE_KINGSIDE || flags == MoveFlags::CASTLE_QUEENSIDE) {
        uint8_t rookCornerPos = (flags == MoveFlags::CASTLE_KINGSIDE)
                                    ? (to + Direction::EAST)
                                    : (to + Direction::WEST + Direction::WEST);
        uint8_t rookTo =
            (flags == MoveFlags::CASTLE_KINGSIDE) ? (to + Direction::WEST) : (to + Direction::EAST);
        setSquare(to, movingPiece.color, Piece::KING);
        setSquare(rookTo, movingPiece.color, Piece::ROOK);
        setSquare(from, Color::NO_COLOR, Piece::NO_PIECE);
        setSquare(rookCornerPos, Color::NO_COLOR, Piece::NO_PIECE);
        hash ^= zobrist->zobristKeys[playerColor][movingPiece.piece][from];
        hash ^= zobrist->zobristKeys[playerColor][Piece::ROOK][rookCornerPos];
        hash ^= zobrist->zobristKeys[playerColor][Piece::ROOK][rookTo];
        hash ^= zobrist->zobristKeys[playerColor][movingPiece.piece][to];
        plies_since_capture += 1;
    } else if (flags == MoveFlags::EN_PASSANT) {
        Offset epOpponent = from;
        if (to > from) {
            // WHITE
            if (to == (from + Direction::NORTH_WEST)) {
                epOpponent -= 1;
            } else {
                epOpponent += 1;
            }
        } else {
            if (to == (from + Direction::SOUTH_WEST)) {
                epOpponent -= 1;
            } else {
                epOpponent += 1;
            }
        }
        setSquare(to, movingPiece.color, movingPiece.piece);
        setSquare(from, Color::NO_COLOR, Piece::NO_PIECE);
        setSquare(epOpponent, Color::NO_COLOR, Piece::NO_PIECE);
        hash ^= zobrist->zobristKeys[playerColor][movingPiece.piece][from];
        hash ^= zobrist->zobristKeys[playerColor][movingPiece.piece][to];
        hash ^= zobrist->zobristKeys[opponentPlayerColor][leavingPiece.piece][epOpponent];
        plies_since_capture = 0;
    } else if (m.getFlags() == MoveFlags::NULL_MOVE) {
        plies_since_capture += 1;
        hash ^= zobrist->zobristSide;
    }
    hash ^= zobrist->zobristSide;
    plies += 1;
    opponentPlayerColor = playerColor;
    playerColor = (playerColor == Color::WHITE ? Color::BLACK : Color::WHITE);
}

void Position::unmakeMove(UndoInfo undoInfo) {
    hash = undoInfo.hash;
    castle_rights[Color::WHITE][Castle::KINGSIDE] =
        undoInfo.castle_rights[Color::WHITE][Castle::KINGSIDE];
    castle_rights[Color::WHITE][Castle::QUEENSIDE] =
        undoInfo.castle_rights[Color::WHITE][Castle::QUEENSIDE];
    castle_rights[Color::BLACK][Castle::KINGSIDE] =
        undoInfo.castle_rights[Color::BLACK][Castle::KINGSIDE];
    castle_rights[Color::BLACK][Castle::QUEENSIDE] =
        undoInfo.castle_rights[Color::BLACK][Castle::QUEENSIDE];
    en_passant = undoInfo.en_passant;
    plies_since_capture = undoInfo.plies_since_capture;
    Move m = undoInfo.move;
    SquareIndex from = m.getFrom();
    SquareIndex to = m.getTo();
    uint8_t flags = m.getFlags();
    SquareInfo movingPiece = board[to];
    Piece leavingPiece = undoInfo.previousPiece;
    if (flags == MoveFlags::QUIET || flags == MoveFlags::DOUBLE_PUSH) {
        setSquare(from, movingPiece.color, movingPiece.piece);
        setSquare(to, Color::NO_COLOR, Piece::NO_PIECE);
    } else if (flags == MoveFlags::CAPTURE) {
        setSquare(to, movingPiece.color == Color::WHITE ? Color::BLACK : Color::WHITE,
                  leavingPiece);
        setSquare(from, movingPiece.color, movingPiece.piece);
    } else if (flags == MoveFlags::CASTLE_KINGSIDE || flags == MoveFlags::CASTLE_QUEENSIDE) {
        uint8_t rookCornerPos = (flags == MoveFlags::CASTLE_KINGSIDE)
                                    ? (to + Direction::EAST)
                                    : (to + Direction::WEST + Direction::WEST);
        uint8_t rookFrom =
            (flags == MoveFlags::CASTLE_KINGSIDE) ? (to + Direction::WEST) : (to + Direction::EAST);
        setSquare(to, Color::NO_COLOR, Piece::NO_PIECE);
        setSquare(rookFrom, Color::NO_COLOR, Piece::NO_PIECE);
        setSquare(from, movingPiece.color, Piece::KING);
        setSquare(rookCornerPos, movingPiece.color, Piece::ROOK);
    } else if (flags == MoveFlags::EN_PASSANT) {
        Offset epOpponent = from;
        if (to > from) {
            // WHITE
            if (to == (from + Direction::NORTH_WEST)) {
                epOpponent -= 1;
            } else {
                epOpponent += 1;
            }
        } else {
            if (to == (from + Direction::SOUTH_WEST)) {
                epOpponent -= 1;
            } else {
                epOpponent += 1;
            }
        }
        setSquare(to, Color::NO_COLOR, Piece::NO_PIECE);
        setSquare(from, movingPiece.color, movingPiece.piece);
        setSquare(epOpponent, movingPiece.color == Color::WHITE ? Color::BLACK : Color::WHITE,
                  Piece::PAWN);
    } else if (flags == MoveFlags::NULL_MOVE) {
    }
    plies -= 1;
    opponentPlayerColor = playerColor;
    playerColor = (playerColor == Color::WHITE ? Color::BLACK : Color::WHITE);
}

void Position::generatePieceMoves(MoveList &moves, MoveList &captures) {
    Bitboard ours = occupation[playerColor];
    Bitboard theirs = occupation[opponentPlayerColor];
    Bitboard oursOrTheirs = ours | theirs;
    Bitboard neitherOursAndTheirs = ~oursOrTheirs;
    Bitboard knights = bitboards[playerColor][Piece::KNIGHT];
    Bitboard bishop = bitboards[playerColor][Piece::BISHOP];
    Bitboard rook = bitboards[playerColor][Piece::ROOK];
    Bitboard queen = bitboards[playerColor][Piece::QUEEN];
    Bitboard king = bitboards[playerColor][Piece::KING];

    Bitboard pawns = bitboards[playerColor][Piece::PAWN];
    Bitboard pawns_promoting;
    Bitboard pawns_not_promoting;
    Bitboard doublePushRank;
    if (playerColor == Color::WHITE) {
        pawns_not_promoting = pawns & notRank7;
        pawns_promoting = pawns & rank7;
        doublePushRank = rank4;
    } else {
        pawns_not_promoting = pawns & notRank2;
        pawns_promoting = pawns & rank2;
        doublePushRank = rank5;
    }
    while (bishop) {
        SquareIndex from = get_ls1b_index(bishop);
        Bitboard result = bishopMagics[from].getAttack(oursOrTheirs);
        Bitboard attacks = result & theirs;
        Bitboard quiet = result & neitherOursAndTheirs;
        captures.addMoves(from, attacks, MoveFlags::CAPTURE);
        moves.addMoves(from, quiet, MoveFlags::QUIET);
        bishop &= unmaskedSquare[from];
    }
    while (knights) {
        SquareIndex from = get_ls1b_index(knights);
        Bitboard attacks = knightAttacks[from] & theirs;
        Bitboard non_attacks = knightAttacks[from] & neitherOursAndTheirs;
        captures.addMoves(from, attacks, MoveFlags::CAPTURE);
        moves.addMoves(from, non_attacks, MoveFlags::QUIET);
        knights &= unmaskedSquare[from];
    }
    while (pawns_promoting) {
        SquareIndex from = get_ls1b_index(pawns_promoting);
        Bitboard attacks = occupation[opponentPlayerColor] & pawnAttacks[playerColor][from];
        Bitboard pushes = neitherOursAndTheirs & pawnPushes[playerColor][from];
        captures.addMoves(from, attacks, MoveFlags::CAPTURE);
        moves.addMoves(from, pushes, MoveFlags::QUIET);
        pawns_promoting &= unmaskedSquare[from];
    }
    while (pawns_not_promoting) {
        SquareIndex from = get_ls1b_index(pawns_not_promoting);
        Bitboard theoreticalAttackingMoves = pawnAttacks[playerColor][from];
        Bitboard realAttackingMoves = theirs & theoreticalAttackingMoves;
        if (en_passant != Square::SQUARE_NONE) {
            Bitboard epBitboard = maskedSquare[en_passant];
            if (theoreticalAttackingMoves & epBitboard) {
                captures.push_back(Move(from, en_passant, MoveFlags::EN_PASSANT));
            }
        }
        captures.addMoves(from, realAttackingMoves, MoveFlags::CAPTURE);
        Bitboard pushes = pawnPushes[playerColor][from] & neitherOursAndTheirs;
        if (pushes) {
            SquareIndex to = get_ls1b_index(pushes);
            moves.push_back(Move(from, to, MoveFlags::QUIET));

            Bitboard doublePush = pawnPushes[playerColor][to];
            if (doublePush & neitherOursAndTheirs & doublePushRank) {
                moves.push_back(Move(from, get_ls1b_index(doublePush), MoveFlags::DOUBLE_PUSH));
            }
        }
        pawns_not_promoting &= unmaskedSquare[from];
    }
    while (queen) {
        SquareIndex from = get_ls1b_index(queen);
        Bitboard result =
            rookMagics[from].getAttack(oursOrTheirs) | bishopMagics[from].getAttack(oursOrTheirs);
        Bitboard attacks = result & theirs;
        Bitboard quiet = result & neitherOursAndTheirs;
        captures.addMoves(from, attacks, MoveFlags::CAPTURE);
        moves.addMoves(from, quiet, MoveFlags::QUIET);
        queen &= unmaskedSquare[from];
    }
    while (rook) {
        SquareIndex from = get_ls1b_index(rook);
        Bitboard result = rookMagics[from].getAttack(oursOrTheirs);
        Bitboard attacks = result & theirs;
        Bitboard quiet = result & neitherOursAndTheirs;
        captures.addMoves(from, attacks, MoveFlags::CAPTURE);
        moves.addMoves(from, quiet, MoveFlags::QUIET);
        rook &= unmaskedSquare[from];
    }
    if (king) {
        SquareIndex from = get_ls1b_index(king);
        Bitboard attacks = kingAttacks[from] & theirs;
        Bitboard non_attacks = kingAttacks[from] & neitherOursAndTheirs;
        captures.addMoves(from, attacks, MoveFlags::CAPTURE);
        moves.addMoves(from, non_attacks, MoveFlags::QUIET);
        for (auto castleType : {Castle::KINGSIDE, Castle::QUEENSIDE}) {
            if (!castle_rights[playerColor][castleType]) {
                continue;
            }
            Position intermediate = *this;
            SquareIndex moveDir =
                (castleType == Castle::KINGSIDE) ? Direction::EAST : Direction::WEST;
            SquareIndex moveDirTarget =
                moveDir + ((castleType == Castle::KINGSIDE) ? Direction::EAST : Direction::WEST);
            SquareIndex moveDirTarget2 = moveDir + Direction::WEST;
            if (((king << moveDir) & neitherOursAndTheirs) &&
                ((king << moveDirTarget) & neitherOursAndTheirs) &&
                (castleType == Castle::KINGSIDE ||
                 ((king << moveDirTarget2) & neitherOursAndTheirs)) &&
                !inCheck(playerColor)) {
                intermediate.setSquare(from + moveDir, playerColor, Piece::KING);
                intermediate.setSquare(from, Color::NO_COLOR, Piece::NO_PIECE);
                if (!intermediate.inCheck(intermediate.playerColor)) {
                    intermediate.setSquare(from + moveDirTarget, playerColor, Piece::KING);
                    intermediate.setSquare(from + moveDir, Color::NO_COLOR, Piece::NO_PIECE);
                    if (!intermediate.inCheck(intermediate.playerColor)) {
                        moves.push_back(Move(from, from + moveDirTarget,
                                             castleType == Castle::KINGSIDE
                                                 ? MoveFlags::CASTLE_KINGSIDE
                                                 : MoveFlags::CASTLE_QUEENSIDE));
                    }
                }
            }
        }
    }
}

bool Position::inCheck(Color side) {
    Color opponentSide = (side == Color::BLACK ? Color::WHITE : Color::BLACK);
    SquareIndex square = get_ls1b_index(bitboards[side][Piece::KING]);
    Bitboard oursAndTheirs = occupation[Color::WHITE] | occupation[Color::BLACK];
    auto opponentPieces = bitboards[opponentSide];
    Bitboard knight = knightAttacks[square] & opponentPieces[Piece::KNIGHT];
    Bitboard rook = rookMagics.at(square).getAttack(oursAndTheirs) &
                    (opponentPieces[Piece::QUEEN] | opponentPieces[Piece::ROOK]);
    Bitboard bishop = bishopMagics.at(square).getAttack(oursAndTheirs) &
                      (opponentPieces[Piece::QUEEN] | opponentPieces[Piece::BISHOP]);
    Bitboard pawn = pawnAttacks[side][square] & opponentPieces[Piece::PAWN];
    Bitboard king = kingAttacks[square] & opponentPieces[Piece::KING];
    return (knight | rook | bishop | pawn | king) > emptyBitboard;
}

void Position::generatePawnMoves(MoveList &moves) {
    Bitboard pawns = bitboards[playerColor][Piece::PAWN];
    Bitboard pawns_not_promoting;
    Bitboard pawns_promoting;
    Bitboard doublePushRank;
    if (playerColor == Color::WHITE) {
        pawns_not_promoting = pawns & notRank7;
        pawns_promoting = pawns & rank7;
        doublePushRank = rank4;
    } else {
        pawns_not_promoting = pawns & notRank2;
        pawns_promoting = pawns & rank2;
        doublePushRank = rank5;
    }
    while (pawns_not_promoting) {
        SquareIndex from = get_ls1b_index(pawns_not_promoting);
        Bitboard attacks = occupation[opponentPlayerColor] & pawnAttacks[playerColor][from];
        moves.addMoves(from, attacks, MoveFlags::CAPTURE);
        if (en_passant != Square::SQUARE_NONE) {
            Bitboard ep = maskedSquare[en_passant];
            if (attacks & ep) {
                moves.push_back(Move(from, en_passant, MoveFlags::EN_PASSANT));
            }
        }
        Bitboard empty = ~(occupation[opponentPlayerColor] | occupation[playerColor]);
        Bitboard pushes = pawnPushes[playerColor][from] & empty;
        while (pushes) {
            SquareIndex to = get_ls1b_index(pushes);
            Bitboard doublePush = pawnPushes[playerColor][to];
            if (doublePush & empty & doublePushRank) {
                moves.push_back(Move(from, get_ls1b_index(doublePush), MoveFlags::QUIET));
            }
            moves.push_back(Move(from, to, MoveFlags::QUIET));
            pushes &= unmaskedSquare[to];
        }
        pawns_not_promoting &= unmaskedSquare[from];
    }
    while (pawns_promoting) {
        SquareIndex from = get_ls1b_index(pawns_promoting);
        Bitboard attacks = occupation[opponentPlayerColor] & pawnAttacks[playerColor][from];
        moves.addMoves(from, attacks, MoveFlags::CAPTURE);
        pawns_promoting &= unmaskedSquare[from];
    }
}

void Position::generateMoves(MoveList &moves, MoveList &captures) {
    generatePieceMoves(moves, captures);
    // generatePawnMoves(moves);
}