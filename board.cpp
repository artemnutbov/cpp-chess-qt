#include "board.h"

#include <QDebug>
#include <algorithm>

#include "bishop.h"
#include "king.h"
#include "knight.h"
#include "pawn.h"
#include "queen.h"
#include "random"
#include "rook.h"
uint64_t Board::zobrist_pieces_[12][64];
uint64_t Board::zobrist_side_to_move_;
uint64_t Board::zobrist_castling_[4];
uint64_t Board::zobrist_en_passant_[8];

Board::Board() : game_result_status_(GameResultStatus::kNotStarted) {
    board_.fill(Figures::kNone);
    is_in_start_pos_board_.fill(true);
}

int Board::GetFigureValue(Figures figure) {
    int value = 0;
    switch (figure) {
        case Figures::kWhitePawn:
            value = 100;
            break;
        case Figures::kWhiteKnight:
            value = 320;
            break;
        case Figures::kWhiteBishop:
            value = 330;
            break;
        case Figures::kWhiteRook:
            value = 500;
            break;
        case Figures::kWhiteQueen:
            value = 900;
            break;
        case Figures::kBlackPawn:
            value = -100;
            break;
        case Figures::kBlackKnight:
            value = -320;
            break;
        case Figures::kBlackBishop:
            value = -330;
            break;
        case Figures::kBlackRook:
            value = -500;
            break;
        case Figures::kBlackQueen:
            value = -900;
            break;
        default:
            break;
    }
    return value;
}
// test version bonus table
const int pawn_table[64] = {0,  0,  0,  0,   0,   0,  0,  0,  50, 50, 50,  50, 50, 50,  50, 50,
                            10, 10, 20, 30,  30,  20, 10, 10, 5,  5,  10,  25, 25, 10,  5,  5,
                            0,  0,  0,  20,  20,  0,  0,  0,  5,  -5, -10, 0,  0,  -10, -5, 5,
                            5,  10, 10, -20, -20, 10, 10, 5,  0,  0,  0,   0,  0,  0,   0,  0};

const int knight_table[64] = {
    -20, -10, -10, -10, -10, -10, -10, -20, -10, 0,   0,   0,   0,   0,   0,   -10,
    -10, 0,   5,   5,   5,   5,   0,   -10, -10, 0,   5,   10,  10,  5,   0,   -10,
    -10, 0,   5,   10,  10,  5,   0,   -10, -10, 0,   5,   5,   5,   5,   0,   -10,
    -10, 0,   0,   0,   0,   0,   0,   -10, -20, -10, -10, -10, -10, -10, -10, -20,
};

const int king_table[64] = {-30, -40, -40, -50, -50, -40, -40, -30, -30, -40, -40, -50, -50,
                            -40, -40, -30, -30, -40, -40, -50, -50, -40, -40, -30, -30, -40,
                            -40, -50, -50, -40, -40, -30, -20, -30, -30, -40, -40, -30, -30,
                            -20, -10, -20, -20, -20, -20, -20, -20, -10, 20,  20,  0,   0,
                            0,   0,   20,  20,  20,  30,  10,  0,   0,   10,  30,  20};

int Board::QuiescenceSearch(int alpha, int beta) {
    // bool start_turn = is_white_turn_to_move_;

    int stand_pat = Evaluate();
    if (stand_pat >= beta) return beta;
    if (stand_pat > alpha) alpha = stand_pat;

    auto moves = GenerateCaptures();
    std::sort(moves.begin(), moves.end(),
              [&](const Move& a, const Move& b) { return RankMove(a) > RankMove(b); });

    for (const auto& move : moves) {
        MakeBotMove(move);
        int score = -QuiescenceSearch(-beta, -alpha);
        UndoMove();

        // if (is_white_turn_to_move_ != start_turn) {
        //     qDebug() << "CRITICAL ERROR: is_white_turn_to_move_ broken after move " << move.from
        //     << "to" << move.to;
        //     is_white_turn_to_move_ = start_turn;
        // }

        if (score >= beta) return beta;
        if (score > alpha) alpha = score;
    }

    return alpha;
}

int Board::RankMove(const Move& move) {
    if (move == best_root_move_) {
        return 30000;
    }
    if (move.type == MoveTypes::kCapture || move.type == MoveTypes::kPromoteCapture) {
        // most valuable victim - least valuable attacker
        int victim_value = GetFigureValue(board_[move.to]);
        int attacker_value = GetFigureValue(board_[move.from]);

        return 10 * victim_value - attacker_value + 10000;  // +10000 bc captures are first
    }

    if (move.type == MoveTypes::kPromoteToEmptySquare) {
        return 9000;
    }

    return 0;
}

int Board::Evaluate() {
    int score = 0;

    for (int i = 0; i < 64; ++i) {
        if (board_[i] == Figures::kNone) continue;

        int material = GetFigureValue(board_[i]);
        int position = 0;

        bool is_white = Config::FigureToSide(board_[i]);
        if (!is_white_pov_) is_white = !is_white;
        // for black we must mirror the index (i ^ 56) to flip the board
        int square_idx = is_white ? i : (i ^ 56);

        switch (board_[i]) {
            case Figures::kWhitePawn:
                position = pawn_table[square_idx];
                break;
            case Figures::kBlackPawn:
                position = -pawn_table[square_idx];
                break;
            case Figures::kWhiteKnight:
                position = knight_table[square_idx];
                break;
            case Figures::kBlackKnight:
                position = -knight_table[square_idx];
                break;
            case Figures::kWhiteKing:
                position = king_table[square_idx];
                break;
            case Figures::kBlackKing:
                position = -king_table[square_idx];
                break;
            // maybe need to add other figure
            default:
                position = 0;
                break;
        }

        score += material + position;
    }

    return is_white_turn_to_move_ ? score : -score;
}

long long Board::Perft(int depth) {
    if (depth == 0) return 1;

    long long nodes = 0;
    auto moves = GenerateMoves();

    for (const auto& move : moves) {
        MakeBotMove(move);
        nodes += Perft(depth - 1);
        UndoMove();
    }
    return nodes;
}

MoveList Board::GenerateCaptures() {
    MoveList move_list;
    for (int i = 0; i < 64; ++i) {
        if (board_[i] == Figures::kNone) continue;

        bool is_white = Config::FigureToSide(board_[i]);

        if (is_white == is_white_turn_to_move_) {
            AllFigureMove(i);

            for (auto const& [target_square, type] : index_pair_map_) {
                if (type != MoveTypes::kCapture && type != MoveTypes::kPromoteCapture &&
                    type != MoveTypes::kEnPassant) {
                    continue;
                }
                // if (target_square < 0)
                //     qDebug() << "i: " << i << " to: " << target_square << "figure type"
                //              << (int)board_[i];
                Move m;
                m.from = i;
                m.to = target_square;
                m.type = type;
                move_list.push_back(m);
            }

            index_pair_map_.clear();
        }
    }

    return move_list;
}

MoveList Board::GenerateMoves() {
    MoveList move_list;

    for (int i = 0; i < 64; ++i) {
        if (board_[i] == Figures::kNone) continue;

        bool is_white = Config::FigureToSide(board_[i]);

        if (is_white == is_white_turn_to_move_) {
            AllFigureMove(i);

            for (auto const& [target_square, type] : index_pair_map_) {
                Move m;
                m.from = i;
                m.to = target_square;
                m.type = type;
                move_list.push_back(m);
            }

            index_pair_map_.clear();
        }
    }

    return move_list;
}

int Board::Negamax(int depth, int alpha, int beta) {
    if (depth == 0) return QuiescenceSearch(alpha, beta);

    auto moves = GenerateMoves();
    std::sort(moves.begin(), moves.end(),
              [&](const Move& a, const Move& b) { return RankMove(a) > RankMove(b); });
    if (moves.empty()) {
        int king_idx = is_white_turn_to_move_ ? white_king_index_ : black_king_index_;
        bool in_check =
            King::IsKingUnderAttack(board_, king_idx, is_white_pov_, is_white_turn_to_move_);
        if (in_check) {
            // checkmate
            return -49000 - depth;
        } else {
            // stalemate
            return 0;
        }
    }
    // auto penultimate_it = history_.back();
    // if (penultimate_it.additional_figure != Figures::kNone ||
    //     penultimate_it.our_figure == Figures::kWhitePawn ||
    //     penultimate_it.our_figure == Figures::kBlackPawn) {
    //     count_50rule_draw_ = 0;
    // }
    // if (count_50rule_draw_ == 100 ) {
    //     return 0;
    // }
    if (IsRepetition()) return 0;

    int max_score = -50000;

    for (auto& move : moves) {
        MakeBotMove(move);

        // recursive call
        int score = -Negamax(depth - 1, -beta, -alpha);

        UndoMove();

        if (score > max_score) {
            max_score = score;
            if (depth == current_root_depth_) {
                current_depth_best_move_ = move;  // found a better move for THIS depth!
            }
        }

        // alpha beta logic
        if (score > alpha) {
            alpha = score;
        }
        if (alpha >= beta) {
            break;  // prune
        }
    }

    return max_score;
}

Move Board::SearchRoot(int max_time_ms) {
    best_root_move_ = Move();
    Move final_best_move;
    auto start_time = std::chrono::high_resolution_clock::now();
    // iterative deepening loop
    for (int depth = 1; depth <= 100; ++depth) {
        current_root_depth_ = depth;

        int score = Negamax(depth, -50000, 50000);

        final_best_move = current_depth_best_move_;
        best_root_move_ = final_best_move;
        auto end = std::chrono::high_resolution_clock::now();
        auto duration =
            std::chrono::duration_cast<std::chrono::milliseconds>(end - start_time).count();

        qDebug() << "Depth" << depth << " finished. Score:" << score << " Time:" << duration
                 << "ms";

        // if we have used more than 25% of our time
        if (duration * 4 > max_time_ms) {
            break;
        }
    }
    return final_best_move;  // returns the move from the last completed depth
}

void Board::MakeBotMove(Move bot_move) {
    int start_square = bot_move.from;
    int new_square = bot_move.to;
    if (start_square == new_square || new_square < 0 || new_square > 63) {
        qDebug() << "Bot cannot move! Game Over or Bug. start_square" << start_square
                 << " new_square: " << new_square << "type " << int(bot_move.type);
        return;
    }
    MoveUndoInfo current_move;
    current_move.from_square = start_square;
    current_move.to_square = new_square;
    current_move.our_figure = board_[start_square];
    current_move.move_type = bot_move.type;
    current_move.hash_snapshot = current_hash_;

    if (bot_move.type == MoveTypes::kCapture) {
        current_move.additional_figure = board_[new_square];
        board_[new_square] = Figures::kNone;
    } else if (bot_move.type == MoveTypes::kPromoteCapture ||
               bot_move.type == MoveTypes::kPromoteToEmptySquare) {
        current_move.additional_figure = board_[new_square];
        bool is_white = Config::FigureToSide(board_[start_square]);
        if (is_white)
            board_[start_square] = Figures::kWhiteQueen;
        else
            board_[start_square] = Figures::kBlackQueen;
    } else if (bot_move.type == MoveTypes::kShortCastling) {
        bool is_white = Config::FigureToSide(board_[start_square]);
        if (is_white_pov_) {
            std::swap(board_[(start_square >> 3) * 8 + 7],
                      board_[(start_square >> 3) * 8 + (new_square & 7) - 1]);
            int rook_start_index = is_white ? 3 : 5;  // RightBottomRook or RightTopRook
            is_in_start_pos_board_[rook_start_index] = false;
        } else {
            std::swap(board_[(start_square >> 3) * 8],
                      board_[(start_square >> 3) * 8 + (new_square & 7) + 1]);
            int rook_start_index = !is_white ? 2 : 4;  // LeftBottomRook or LeftTopRook
            is_in_start_pos_board_[rook_start_index] = false;
        }
    } else if (bot_move.type == MoveTypes::kLongCastling) {
        bool is_white = Config::FigureToSide(board_[start_square]);
        if (is_white_pov_) {
            std::swap(board_[(start_square >> 3) * 8],
                      board_[(start_square >> 3) * 8 + (new_square & 7) + 1]);
            int rook_start_index = is_white ? 2 : 4;  // LeftBottomRook or LeftTopRook
            is_in_start_pos_board_[rook_start_index] = false;
        } else {
            std::swap(board_[(start_square >> 3) * 8 + 7],
                      board_[(start_square >> 3) * 8 + (new_square & 7) - 1]);
            int rook_start_index = !is_white ? 3 : 5;  // RightBottomRook or RightTopRook
            is_in_start_pos_board_[rook_start_index] = false;
        }
    } else if (bot_move.type == MoveTypes::kEnPassant) {
        if ((new_square >> 3) == 2) {
            current_move.additional_figure = board_[new_square + 8];
            board_[new_square + 8] = Figures::kNone;
        } else {
            current_move.additional_figure = board_[new_square - 8];
            board_[new_square - 8] = Figures::kNone;
        }
    }
    is_white_turn_to_move_ = !is_white_turn_to_move_;
    ++count_50rule_draw_;

    if (current_move.our_figure == Figures::kWhiteKing) {
        white_king_index_ = new_square;
    } else if (current_move.our_figure == Figures::kBlackKing) {
        black_king_index_ = new_square;
    } else if (current_move.our_figure == Figures::kBlackRook ||
               current_move.our_figure == Figures::kWhiteRook) {
        switch (start_square) {
            case 0:
                is_in_start_pos_board_[4] = false;  // LeftTopRook
                break;
            case 7:
                is_in_start_pos_board_[5] = false;  // RightTopRook
                break;
            case 56:
                is_in_start_pos_board_[2] = false;  // LeftBottomRook
                break;
            case 63:
                is_in_start_pos_board_[3] = false;  // RightBottomRook
                break;
            default:
                break;
        }
    }
    board_[new_square] = board_[start_square];
    board_[start_square] = Figures::kNone;

    history_.push_back(current_move);
    current_hash_ = ComputeHash();
}

Figures Board::NameToFigure(FiguresName name, bool is_white) {
    switch (name) {
        case FiguresName::kBishop:
            return is_white ? Figures::kWhiteBishop : Figures::kBlackBishop;
        case FiguresName::kRook:
            return is_white ? Figures::kWhiteRook : Figures::kBlackRook;
        case FiguresName::kKnight:
            return is_white ? Figures::kWhiteKnight : Figures::kBlackKnight;
        case FiguresName::kPawn:
            return is_white ? Figures::kWhitePawn : Figures::kBlackPawn;
        case FiguresName::kKing:
            return is_white ? Figures::kWhiteKing : Figures::kBlackKing;
        case FiguresName::kQueen:
            return is_white ? Figures::kWhiteQueen : Figures::kBlackQueen;
        default:
            break;
    }
    return is_white ? Figures::kWhiteQueen : Figures::kBlackQueen;
}

void Board::InitZobrist() {
    // fixed seed
    std::mt19937_64 rng(123456789);

    for (int p = 0; p < 12; ++p) {
        for (int s = 0; s < 64; ++s) {
            zobrist_pieces_[p][s] = rng();
        }
    }
    zobrist_side_to_move_ = rng();
    for (int i = 0; i < 4; ++i) zobrist_castling_[i] = rng();
    for (int i = 0; i < 8; ++i) zobrist_en_passant_[i] = rng();
}

uint64_t Board::ComputeHash() {
    uint64_t hash = 0;

    for (int i = 0; i < 64; ++i) {
        Figures fig = board_[i];
        if (fig == Figures::kNone) continue;

        // map enum to 0-11 index
        int piece_index = 0;
        switch (fig) {
            case Figures::kWhitePawn:
                piece_index = 0;
                break;
            case Figures::kWhiteKnight:
                piece_index = 1;
                break;
            case Figures::kWhiteBishop:
                piece_index = 2;
                break;
            case Figures::kWhiteRook:
                piece_index = 3;
                break;
            case Figures::kWhiteQueen:
                piece_index = 4;
                break;
            case Figures::kWhiteKing:
                piece_index = 5;
                break;
            case Figures::kBlackPawn:
                piece_index = 6;
                break;
            case Figures::kBlackKnight:
                piece_index = 7;
                break;
            case Figures::kBlackBishop:
                piece_index = 8;
                break;
            case Figures::kBlackRook:
                piece_index = 9;
                break;
            case Figures::kBlackQueen:
                piece_index = 10;
                break;
            case Figures::kBlackKing:
                piece_index = 11;
                break;
            default:
                break;
        }
        hash ^= zobrist_pieces_[piece_index][i];
    }

    if (!is_white_turn_to_move_) {
        hash ^= zobrist_side_to_move_;
    }

    //  56=WR_A, 63=WR_H, 60=WK, 0=BR_A, 7=BR_H, 4=BK

    // White King Side

    if (board_[63] == Figures::kWhiteRook && !is_in_start_pos_board_[63] &&
        board_[60] == Figures::kWhiteKing && !is_in_start_pos_board_[60])
        hash ^= zobrist_castling_[0];

    // White Queen Side
    if (board_[56] == Figures::kWhiteRook && !is_in_start_pos_board_[56] &&
        board_[60] == Figures::kWhiteKing && !is_in_start_pos_board_[60])
        hash ^= zobrist_castling_[1];

    // Black King Side
    if (board_[7] == Figures::kBlackRook && !is_in_start_pos_board_[7] &&
        board_[4] == Figures::kBlackKing && !is_in_start_pos_board_[4])
        hash ^= zobrist_castling_[2];

    // Black Queen Side
    if (board_[0] == Figures::kBlackRook && !is_in_start_pos_board_[0] &&
        board_[4] == Figures::kBlackKing && !is_in_start_pos_board_[4])
        hash ^= zobrist_castling_[3];

    // 4. En Passant (Check history_ for double pawn push)
    if (!history_.empty()) {
        const auto& last = history_.back();
        if ((last.our_figure == Figures::kWhitePawn || last.our_figure == Figures::kBlackPawn) &&
            std::abs(last.from_square - last.to_square) == 16) {
            int file = last.to_square & 7;
            hash ^= zobrist_en_passant_[file];
        }
    }

    return hash;
}

bool Board::IsRepetition() const {
    int count = 0;

    for (auto it = history_.rbegin(); it != history_.rend(); ++it) {
        if (it->hash_snapshot == current_hash_) {
            count++;
        }

        // stop checking if we hit a "irreversible" move
        if (it->move_type == MoveTypes::kCapture || it->our_figure == Figures::kWhitePawn ||
            it->our_figure == Figures::kBlackPawn) {
            break;
        }
    }

    // current state matches (1) + found in history_ (2) = 3 total
    return count >= 2;
}

void Board::SetUp(bool white_pov) {
    history_.reserve(100);  // reserve a little bigger than avg moves
    is_white_pov_ = white_pov;
    game_result_status_ = GameResultStatus::kPlayingNow;
    int white_pov_pieces_rank = is_white_pov_ ? 7 * 8 : 0;
    int black_pov_pieces_rank = !is_white_pov_ ? 7 * 8 : 0;

    int white_pov_pawns_rank = is_white_pov_ ? 6 * 8 : 1 * 8;
    int black_pov_pawns_rank = !is_white_pov_ ? 6 * 8 : 1 * 8;

    white_king_index_ = is_white_pov_ ? white_pov_pieces_rank + 4 : white_pov_pieces_rank + 3;
    black_king_index_ = is_white_pov_ ? black_pov_pieces_rank + 4 : black_pov_pieces_rank + 3;

    board_[white_pov_pieces_rank + 2] = Figures::kWhiteBishop;
    board_[white_pov_pieces_rank + 5] = Figures::kWhiteBishop;

    board_[white_pov_pieces_rank + 1] = Figures::kWhiteKnight;
    board_[white_pov_pieces_rank + 6] = Figures::kWhiteKnight;

    board_[white_pov_pieces_rank + 0] = Figures::kWhiteRook;
    board_[white_pov_pieces_rank + 7] = Figures::kWhiteRook;

    board_[white_king_index_] = Figures::kWhiteKing;
    board_[is_white_pov_ ? white_pov_pieces_rank + 3 : white_pov_pieces_rank + 4] =
        Figures::kWhiteQueen;

    board_[black_pov_pieces_rank + 0] = Figures::kBlackRook;
    board_[black_pov_pieces_rank + 7] = Figures::kBlackRook;

    board_[black_pov_pieces_rank + 2] = Figures::kBlackBishop;
    board_[black_pov_pieces_rank + 5] = Figures::kBlackBishop;

    board_[black_pov_pieces_rank + 1] = Figures::kBlackKnight;
    board_[black_pov_pieces_rank + 6] = Figures::kBlackKnight;

    board_[black_king_index_] = Figures::kBlackKing;
    board_[is_white_pov_ ? black_pov_pieces_rank + 3 : black_pov_pieces_rank + 4] =
        Figures::kBlackQueen;

    for (size_t i = 0; i < 8; ++i) {
        board_[white_pov_pawns_rank + i] = Figures::kWhitePawn;
        board_[black_pov_pawns_rank + i] = Figures::kBlackPawn;
    }

    current_hash_ = ComputeHash();
}

int Board::GetKingIndex(bool is_white) const {
    return is_white ? white_king_index_ : black_king_index_;
}

bool Board::GetWhitePov() const {
    return is_white_pov_;
}
bool Board::ValidIndex(int i) {
    if (board_[i] != Figures::kNone) return true;
    return false;
}

GameResultStatus Board::GetGameState() const {
    return game_result_status_;
}

Figures Board::GetFigure(int i) {
    return board_[i];
}

MoveMap& Board::GetAllLegalMoves() {
    return index_pair_map_;
}

void Board::SquareMove(int start_square) {
    Figures current_name = board_[start_square];
    if (current_name == Figures::kBlackBishop || current_name == Figures::kWhiteBishop)
        Bishop::GetBishopMoves(index_pair_map_, board_, start_square);
    else if (current_name == Figures::kBlackPawn || current_name == Figures::kWhitePawn) {
        bool is_en_passant = false;
        MoveUndoInfo last_move = history_.back();
        if ((last_move.our_figure == Figures::kBlackPawn ||
             last_move.our_figure == Figures::kWhitePawn)) {
            int distance = abs((last_move.from_square >> 3) - (last_move.to_square >> 3));
            is_en_passant = distance == 2 ? true : false;
        }
        Pawn::GetPawnMoves(index_pair_map_, board_, start_square, last_move.to_square,
                           is_white_pov_, is_en_passant);
    } else if (current_name == Figures::kBlackKnight || current_name == Figures::kWhiteKnight)
        Knight::GetKnightMoves(index_pair_map_, board_, start_square);
    else if (current_name == Figures::kBlackRook || current_name == Figures::kWhiteRook)
        Rook::GetRookMoves(index_pair_map_, board_, start_square);
    else if (current_name == Figures::kBlackQueen || current_name == Figures::kWhiteQueen)
        Queen::GetQueenMoves(index_pair_map_, board_, start_square);
    else  // Figures::kNone never appears here
        King::GetKingMoves(index_pair_map_, board_, is_in_start_pos_board_, start_square,
                           is_white_pov_, Config::FigureToSide(board_[start_square]));
}

void Board::AllFigureMove(int start_square) {
    if (!index_pair_map_.empty()) index_pair_map_.clear();
    if (Config::FigureToSide(board_[start_square]) != is_white_turn_to_move_) return;
    SquareMove(start_square);

    if ((board_[start_square] != Figures::kBlackKing) &&
        (board_[start_square] != Figures::kWhiteKing)) {
        int king_index = (is_white_turn_to_move_) ? white_king_index_ : black_king_index_;
        for (const auto& it : index_pair_map_) {
            if (it.second == MoveTypes::kCapture || it.second == MoveTypes::kPromoteCapture) {
                Figures tmp = board_[it.first];
                board_[it.first] = Figures::kNone;

                std::swap(board_[start_square], board_[it.first]);
                bool is_in_check = King::IsFreeToMove(board_, king_index, is_white_pov_,
                                                      Config::FigureToSide(board_[king_index]));
                std::swap(board_[start_square], board_[it.first]);
                board_[it.first] = tmp;
                if (is_in_check) index_pair_map_.erase(it.first);
            } else {
                std::swap(board_[start_square], board_[it.first]);
                bool is_in_check = King::IsFreeToMove(board_, king_index, is_white_pov_,
                                                      Config::FigureToSide(board_[king_index]));
                std::swap(board_[start_square], board_[it.first]);
                if (is_in_check) index_pair_map_.erase(it.first);
            }
        }
    }
}

void Board::SetResultState() {
    if (!index_pair_map_.empty()) index_pair_map_.clear();
    int king_index = (is_white_turn_to_move_) ? white_king_index_ : black_king_index_;
    bool is_in_check = King::IsKingUnderAttack(board_, king_index, is_white_pov_,
                                               Config::FigureToSide(board_[king_index]));

    game_result_status_ = GameResultStatus::kStalemate;
    for (size_t i = 0; i < 64; ++i) {
        if (board_[i] != Figures::kNone &&
            Config::FigureToSide(board_[i]) == is_white_turn_to_move_) {
            AllFigureMove(i);
            if (!index_pair_map_.empty()) {
                game_result_status_ = GameResultStatus::kPlayingNow;
                break;
            }
        }
    }

    if (is_in_check && game_result_status_ == GameResultStatus::kStalemate) {
        if (is_white_turn_to_move_) {
            game_result_status_ = GameResultStatus::kBlackWin;
            qDebug() << "checkmate for white!!!!!!!";
        } else {
            game_result_status_ = GameResultStatus::kWhiteWin;
            qDebug() << "checkmate for black!!!!!!!";
        }
    } else if (game_result_status_ == GameResultStatus::kStalemate)
        qDebug() << "Stalemate !!!!!!!";
    else if (IsRepetition()) {
        game_result_status_ = GameResultStatus::kStalemate;  // or create a kDrawRepetition status
        qDebug() << "Draw by Repetition!";
        return;
    } else {  // 100 bsc 50 for white and 50 for black
        auto penultimate = history_.back();
        if (penultimate.additional_figure != Figures::kNone ||
            penultimate.our_figure == Figures::kWhitePawn ||
            penultimate.our_figure == Figures::kBlackPawn) {
            count_50rule_draw_ = 0;
        }
        if (count_50rule_draw_ == 100) {
            game_result_status_ = GameResultStatus::kStalemate;  // or create a kDraw50Rules status
            return;
        }
    }
}

bool Board::IsPawnPromote(int i) {
    Figures current_figure = board_[i];
    if ((current_figure == Figures::kWhitePawn || current_figure == Figures::kBlackPawn) &&
        ((i >> 3) == 7 || (i >> 3) == 0)) {
        board_[i] = Figures::kNone;
        promote_pawn_index_ = *index_pair_map_.find(i);
        return true;
    }
    return false;
}

int Board::GetPromoteIndex() const {
    return promote_pawn_index_.first;
}
void Board::Promotion(FiguresName figure_name, bool is_white) {
    Figures figure = NameToFigure(figure_name, is_white);
    board_[promote_pawn_index_.first] = figure;
}
bool Board::ActionMove(int start_square, int new_square) {
    auto it = index_pair_map_.find(new_square);
    if (it == index_pair_map_.end()) {
        return false;
    }
    MoveUndoInfo current_move;
    current_move.from_square = start_square;
    current_move.to_square = new_square;
    current_move.our_figure = board_[start_square];
    current_move.move_type = it->second;
    current_move.hash_snapshot = current_hash_;

    if (it->second == MoveTypes::kCapture || it->second == MoveTypes::kPromoteCapture) {
        current_move.additional_figure = board_[new_square];
        board_[new_square] = Figures::kNone;
    } else if (it->second == MoveTypes::kShortCastling) {
        bool is_white = Config::FigureToSide(board_[start_square]);
        if (is_white_pov_) {
            std::swap(board_[(start_square >> 3) * 8 + 7],
                      board_[(start_square >> 3) * 8 + (new_square & 7) - 1]);
            int rook_start_index = is_white ? 3 : 5;  // RightBottomRook or RightTopRook
            is_in_start_pos_board_[rook_start_index] = false;
        } else {
            std::swap(board_[(start_square >> 3) * 8],
                      board_[(start_square >> 3) * 8 + (new_square & 7) + 1]);
            int rook_start_index = !is_white ? 2 : 4;  // LeftBottomRook or LeftTopRook
            is_in_start_pos_board_[rook_start_index] = false;
        }
    } else if (it->second == MoveTypes::kLongCastling) {
        bool is_white = Config::FigureToSide(board_[start_square]);
        if (is_white_pov_) {
            std::swap(board_[(start_square >> 3) * 8],
                      board_[(start_square >> 3) * 8 + (new_square & 7) + 1]);
            int rook_start_index = is_white ? 2 : 4;  // LeftBottomRook or LeftTopRook
            is_in_start_pos_board_[rook_start_index] = false;
        } else {
            std::swap(board_[(start_square >> 3) * 8 + 7],
                      board_[(start_square >> 3) * 8 + (new_square & 7) - 1]);
            int rook_start_index = !is_white ? 3 : 5;  // RightBottomRook or RightTopRook
            is_in_start_pos_board_[rook_start_index] = false;
        }
    } else if (it->second == MoveTypes::kEnPassant) {
        if ((new_square >> 3) == 2) {
            current_move.additional_figure = board_[new_square + 8];
            board_[new_square + 8] = Figures::kNone;
        } else {
            current_move.additional_figure = board_[new_square - 8];
            board_[new_square - 8] = Figures::kNone;
        }
    }
    is_white_turn_to_move_ = !is_white_turn_to_move_;
    ++count_50rule_draw_;

    if (current_move.our_figure == Figures::kWhiteKing) {
        white_king_index_ = new_square;
        is_in_start_pos_board_[0] = false;
    } else if (current_move.our_figure == Figures::kBlackKing) {
        black_king_index_ = new_square;
        is_in_start_pos_board_[1] = false;
    } else if (current_move.our_figure == Figures::kBlackRook ||
               current_move.our_figure == Figures::kWhiteRook) {
        switch (start_square) {
            case 0:
                is_in_start_pos_board_[4] = false;  // LeftTopRook
                break;
            case 7:
                is_in_start_pos_board_[5] = false;  // RightTopRook
                break;
            case 56:
                is_in_start_pos_board_[2] = false;  // LeftBottomRook
                break;
            case 63:
                is_in_start_pos_board_[3] = false;  // RightBottomRook
                break;
            default:
                break;
        }
    }
    board_[new_square] = board_[start_square];
    board_[start_square] = Figures::kNone;

    history_.push_back(current_move);
    current_hash_ = ComputeHash();
    return true;
}

void Board::UndoMove() {
    if (history_.empty()) return;
    MoveUndoInfo last_move = history_.back();
    std::swap(board_[last_move.to_square], board_[last_move.from_square]);
    // board_[last_move.from_square] = last_move.our_figure;
    is_white_turn_to_move_ = !is_white_turn_to_move_;
    if (last_move.move_type == MoveTypes::kCapture ||
        last_move.move_type == MoveTypes::kPromoteCapture)
        board_[last_move.to_square] = last_move.additional_figure;
    else if (last_move.move_type == MoveTypes::kShortCastling) {
        bool is_white = Config::FigureToSide(last_move.our_figure);
        if (is_white_pov_) {
            std::swap(board_[(last_move.from_square >> 3) * 8 + 7],
                      board_[(last_move.from_square >> 3) * 8 + (last_move.to_square & 7) - 1]);
            int rook_start_index = is_white ? 3 : 5;  // RightBottomRook or RightTopRook
            is_in_start_pos_board_[rook_start_index] = true;
        } else {
            std::swap(board_[(last_move.from_square >> 3) * 8],
                      board_[(last_move.from_square >> 3) * 8 + (last_move.to_square & 7) + 1]);
            int rook_start_index = !is_white ? 2 : 4;  // LeftBottomRook or LeftTopRook
            is_in_start_pos_board_[rook_start_index] = true;
        }
    } else if (last_move.move_type == MoveTypes::kLongCastling) {
        bool is_white = Config::FigureToSide(last_move.our_figure);
        if (is_white_pov_) {
            std::swap(board_[(last_move.from_square >> 3) * 8],
                      board_[(last_move.from_square >> 3) * 8 + (last_move.to_square & 7) + 1]);
            int rook_start_index = is_white ? 2 : 4;  // LeftBottomRook or LeftTopRook
            is_in_start_pos_board_[rook_start_index] = true;
        } else {
            std::swap(board_[(last_move.from_square >> 3) * 8 + 7],
                      board_[(last_move.from_square >> 3) * 8 + (last_move.to_square & 7) - 1]);
            int rook_start_index = !is_white ? 3 : 5;  // RightBottomRook or RightTopRook
            is_in_start_pos_board_[rook_start_index] = true;
        }
    }

    else if (last_move.move_type == MoveTypes::kEnPassant) {
        if ((last_move.to_square >> 3) == 2)
            board_[last_move.to_square + 8] = last_move.additional_figure;
        else
            board_[last_move.to_square - 8] = last_move.additional_figure;
    }
    if (last_move.move_type == MoveTypes::kPromoteToEmptySquare ||
        last_move.move_type == MoveTypes::kPromoteCapture) {
        board_[last_move.from_square] = last_move.our_figure;
    }

    if (last_move.our_figure == Figures::kWhiteKing) {
        white_king_index_ = last_move.from_square;
        bool is_first_move = true;
        for (int i = history_.size() - 3; i >= 0; i -= 2) {
            if (history_[i].our_figure == Figures::kWhiteKing) {
                is_first_move = false;
                break;
            }
        }
        if (is_first_move) is_in_start_pos_board_[0] = true;
    } else if (last_move.our_figure == Figures::kBlackKing) {
        black_king_index_ = last_move.from_square;
        bool is_first_move = true;
        for (int i = history_.size() - 3; i >= 0; i -= 2) {
            if (history_[i].our_figure == Figures::kBlackKing) {
                is_first_move = false;
                break;
            }
        }
        if (is_first_move) is_in_start_pos_board_[1] = true;
    } else if (last_move.our_figure == Figures::kBlackRook ||
               last_move.our_figure == Figures::kWhiteRook) {
        // need to search through all history to find was a move before or not
        std::function<bool(const std::vector<MoveUndoInfo>&, int)> func =
            [](const std::vector<MoveUndoInfo>& history_, int target) {
                for (int i = history_.size() - 3; i >= 0; i -= 2) {
                    if ((history_[i].our_figure == Figures::kBlackRook ||
                         history_[i].our_figure == Figures::kWhiteRook) &&
                        history_[i].from_square == target)
                        return false;
                }
                return true;
            };
        switch (last_move.from_square) {
            case 0:
                if (func(history_, 0)) is_in_start_pos_board_[4] = true;  // LeftTopRook
                break;
            case 7:
                if (func(history_, 7)) is_in_start_pos_board_[5] = true;  // RightTopRook
                break;
            case 56:
                if (func(history_, 56)) is_in_start_pos_board_[2] = true;  // LeftBottomRook
                break;
            case 63:
                if (func(history_, 63)) is_in_start_pos_board_[3] = true;  // RightBottomRook
                break;
            default:
                break;
        }
    }
    if (count_50rule_draw_ != 0) --count_50rule_draw_;
    current_hash_ = last_move.hash_snapshot;
    history_.pop_back();
}
