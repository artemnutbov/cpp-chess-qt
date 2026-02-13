#include "board.h"

#include <QDebug>

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
    // Use a fixed seed so the numbers are the same every time you run the game
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

        // Map enum to 0-11 index
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

    // 3. Castling Rights (Based on your bool flag: true = has moved)
    // Assuming indices: 56=WR_A, 63=WR_H, 60=WK, 0=BR_A, 7=BR_H, 4=BK
    // Note: Adjust indices based on your POV logic if needed

    // White King Side (Rook H1 didn't move, King didn't move)

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
        // If last move was a double pawn push, add the FILE of that pawn
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

    // Iterate backwards through history_
    for (auto it = history_.rbegin(); it != history_.rend(); ++it) {
        // If the hash matches the CURRENT board
        if (it->hash_snapshot == current_hash_) {
            count++;
        }

        // We can stop checking if we hit a "irreversible" move
        if (it->move_type == MoveTypes::kCapture || it->our_figure == Figures::kWhitePawn ||
            it->our_figure == Figures::kBlackPawn) {
            break;
        }
    }

    // Current state matches (1) + found in history_ (2) = 3 total
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
        MoveInfo last_move = *std::prev(history_.end());
        if ((last_move.our_figure == Figures::kBlackPawn ||
             last_move.our_figure == Figures::kWhitePawn)) {
            int distance = abs((last_move.from_square >> 3) - (last_move.to_square >> 3));
            is_en_passant = distance == 2 ? true : false;
        }
        Pawn::GetPawnMoves(index_pair_map_, board_, is_in_start_pos_board_, start_square,
                           last_move.to_square, is_white_pov_, is_en_passant);
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
                bool is_in_check =
                    King::IsKingUnderAttack(board_, king_index, is_white_pov_,
                                            Config::FigureToSide(board_[king_index]));

                std::swap(board_[start_square], board_[it.first]);
                board_[it.first] = tmp;
                if (is_in_check) index_pair_map_.erase(it.first);
            } else {
                std::swap(board_[start_square], board_[it.first]);
                bool is_in_check =
                    King::IsKingUnderAttack(board_, king_index, is_white_pov_,
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
        game_result_status_ = GameResultStatus::kStalemate;  // or create a Draw_Repetition status
        qDebug() << "Draw by Repetition!";
        return;
    } else {  // 100 bsc 50 for white and 50 for black
        auto penultimate_it = std::prev(history_.end());
        if (penultimate_it->additional_figure != Figures::kNone ||
            penultimate_it->our_figure == Figures::kWhitePawn ||
            penultimate_it->our_figure == Figures::kBlackPawn) {
            count_50rule_draw_ = 0;
        }
        if (count_50rule_draw_ == 100) {
            game_result_status_ = GameResultStatus::kStalemate;  // or create a Draw_50rules status
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
    MoveInfo current_move;
    current_move.from_square = start_square;
    current_move.to_square = new_square;
    current_move.our_figure = board_[start_square];
    current_move.move_type = it->second;
    current_move.hash_snapshot = current_hash_;

    is_in_start_pos_board_[new_square] = false;

    if (it->second == MoveTypes::kCapture || it->second == MoveTypes::kPromoteCapture) {
        current_move.additional_figure = board_[new_square];
        board_[new_square] = Figures::kNone;
    } else if (it->second == MoveTypes::kShortCastling) {
        // board[start_square >> 3) * 8].second = false;
        if (is_white_pov_)
            std::swap(board_[(start_square >> 3) * 8 + 7],
                      board_[(start_square >> 3) * 8 + (new_square & 7) - 1]);
        else
            std::swap(board_[(start_square >> 3) * 8],
                      board_[(start_square >> 3) * 8 + (new_square & 7) + 1]);
    } else if (it->second == MoveTypes::kLongCastling) {
        // board[y][7]->handle_move();
        if (is_white_pov_)
            std::swap(board_[(start_square >> 3) * 8],
                      board_[(start_square >> 3) * 8 + (new_square & 7) + 1]);
        else
            std::swap(board_[(start_square >> 3) * 8 + 7],
                      board_[(start_square >> 3) * 8 + (new_square & 7) - 1]);
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
    } else if (current_move.our_figure == Figures::kBlackKing) {
        black_king_index_ = new_square;
    }
    board_[new_square] = board_[start_square];
    board_[start_square] = Figures::kNone;

    history_.push_back(current_move);
    current_hash_ = ComputeHash();
    return true;
}

void Board::UndoMove() {
    if (history_.empty()) return;
    MoveInfo last_move = history_.back();
    std::swap(board_[last_move.to_square], board_[last_move.from_square]);
    if (last_move.move_type == MoveTypes::kCapture ||
        last_move.move_type == MoveTypes::kPromoteCapture)
        board_[last_move.to_square] = last_move.additional_figure;
    else if (last_move.move_type == MoveTypes::kShortCastling) {
        if (is_white_pov_)
            std::swap(board_[(last_move.from_square >> 3) * 8 + 7],
                      board_[(last_move.from_square >> 3) * 8 + (last_move.to_square & 7) - 1]);
        else
            std::swap(board_[(last_move.from_square >> 3) * 8],
                      board_[(last_move.from_square >> 3) * 8 + (last_move.to_square & 7) + 1]);
    } else if (last_move.move_type == MoveTypes::kLongCastling) {
        if (is_white_pov_)
            std::swap(board_[(last_move.from_square >> 3) * 8],
                      board_[(last_move.from_square >> 3) * 8 + (last_move.to_square & 7) + 1]);
        else
            std::swap(board_[(last_move.from_square >> 3) * 8 + 7],
                      board_[(last_move.from_square >> 3) * 8 + (last_move.to_square & 7) - 1]);
    } else if (last_move.move_type == MoveTypes::kEnPassant) {
        if ((last_move.to_square >> 3) == 2)
            board_[last_move.to_square + 8] = last_move.additional_figure;
        else
            board_[last_move.to_square - 8] = last_move.additional_figure;
    }
    if (last_move.move_type == MoveTypes::kMoveToEmptySquare ||
        last_move.move_type == MoveTypes::kPromoteCapture) {
        board_[last_move.from_square] = last_move.our_figure;
    }

    if (last_move.our_figure == Figures::kWhiteKing) {
        white_king_index_ = last_move.from_square;

    } else if (last_move.our_figure == Figures::kBlackKing) {
        black_king_index_ = last_move.from_square;
    }

    is_white_turn_to_move_ = !is_white_turn_to_move_;

    // check if pawn start pos match start game pawn po
    if (last_move.our_figure == Figures::kBlackPawn ||
        last_move.our_figure == Figures::kWhitePawn) {
        bool is_white = Config::FigureToSide(last_move.our_figure);
        bool is_go_up = is_white ? is_white_pov_ : !is_white_pov_;
        if (is_go_up && (last_move.from_square >> 3) == 6 ||
            !is_go_up && (last_move.from_square >> 3) == 1)
            is_in_start_pos_board_[last_move.from_square] = true;
    } else
        is_in_start_pos_board_[last_move.from_square] = true;
    current_hash_ = last_move.hash_snapshot;
    history_.pop_back();
}
