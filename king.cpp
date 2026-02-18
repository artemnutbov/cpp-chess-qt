#include "king.h"

bool King::IsKingUnderAttack(const ArrayBoard& board, int start_square, bool is_white_pov,
                             bool is_white) {
    if (IsAttackedByKnight(board, start_square, is_white) ||
        IsAttackedByBishop(board, start_square, is_white) ||
        IsAttackedByRook(board, start_square, is_white) ||
        IsAttackedByPawn(board, start_square, is_white_pov, is_white) ||
        IsEnemyKingClose(board, start_square, is_white))
        return true;
    return false;
}
bool King::IsFreeToMove(const ArrayBoard& board, int start_square, bool is_white_pov,
                        bool is_white) {
    if (IsAttackedByBishop(board, start_square, is_white) ||
        IsAttackedByRook(board, start_square, is_white) ||
        IsAttackedByKnight(board, start_square, is_white) ||
        IsAttackedByPawn(board, start_square, is_white_pov, is_white))
        return true;
    return false;
}
bool King::IsAttackedByPawn(const ArrayBoard& board, int start_square, bool is_white_pov,
                            bool is_white) {
    bool is_go_up = is_white ? is_white_pov : !is_white_pov;

    is_go_up ? start_square -= 8 : start_square += 8;

    bool is_last_legal_move = false;
    if ((start_square & 7) == 7) is_last_legal_move = true;
    ++start_square;
    if (board[start_square] != Figures::kNone && !is_last_legal_move) {
        if (Config::FigureToSide(board[start_square]) != is_white &&
            (board[start_square] == Figures::kWhitePawn ||
             board[start_square] == Figures::kBlackPawn)) {
            return true;
        }
    }

    is_last_legal_move = false;
    --start_square;
    if ((start_square & 7) == 0) is_last_legal_move = true;
    --start_square;

    if (board[start_square] != Figures::kNone && !is_last_legal_move) {
        if (Config::FigureToSide(board[start_square]) != is_white &&
            (board[start_square] == Figures::kWhitePawn ||
             board[start_square] == Figures::kBlackPawn)) {
            return true;
        }
    }

    return false;
}

bool King::IsAttackedByKnight(const ArrayBoard& board, int start_square, bool is_white) {
    int offsets[] = {-17, -15, -10, -6, 6, 10, 15, 17};
    int move_square = start_square;
    for (int offset : offsets) {
        int index = move_square + offset;

        if (index < 0 || index > 63) continue;

        // did we teleport?
        // if diff > 2, we wrapped around the board edges.
        if (std::abs((move_square & 7) - (index & 7)) > 2) continue;

        Figures current_figure = board[index];
        if (current_figure == Figures::kNone)
            continue;
        else if ((Config::FigureToSide(current_figure) != is_white) &&
                 (current_figure == Figures::kWhiteKnight ||
                  current_figure == Figures::kBlackKnight)) {
            return true;
        }
    }
    return false;
}

bool King::IsAttackedByBishop(const ArrayBoard& board, int start_square, bool is_white) {
    if (WhereIsBishop(board, start_square, 9, is_white) ||
        WhereIsBishop(board, start_square, 7, is_white) ||
        WhereIsBishop(board, start_square, -9, is_white) ||
        WhereIsBishop(board, start_square, -7, is_white))
        return true;
    return false;
}

bool King::WhereIsBishop(const ArrayBoard& board, int start_square, int step, bool is_white) {
    int move_square = start_square;

    bool is_last_legal_move = false;
    if ((step == 9 || step == -7) && (move_square & 7) == 7) is_last_legal_move = true;
    if ((step == 7 || step == -9) && (move_square & 7) == 0) is_last_legal_move = true;
    move_square += step;
    if (move_square < 0 || move_square > 63) return false;
    while (!is_last_legal_move) {
        if ((step == 9 || step == -7) && (move_square & 7) == 7) is_last_legal_move = true;
        if ((step == 7 || step == -9) && (move_square & 7) == 0) is_last_legal_move = true;
        if (move_square < 0 || move_square > 63) return false;
        Figures current_figure = board[move_square];
        if (current_figure == Figures::kNone) {
            move_square += step;
            continue;
        }
        if (is_white) {
            if (current_figure == Figures::kWhiteKing) {
                move_square += step;
                continue;
            }
        } else {
            if (current_figure == Figures::kBlackKing) {
                move_square += step;
                continue;
            }
        }
        if ((Config::FigureToSide(current_figure) != is_white) &&
            (current_figure == Figures::kWhiteBishop || current_figure == Figures::kBlackBishop ||
             current_figure == Figures::kWhiteQueen || current_figure == Figures::kBlackQueen)) {
            return true;
        } else
            return false;

        move_square += step;
    }
    return false;
}

bool King::IsAttackedByRook(const ArrayBoard& board, int start_square, bool is_white) {
    if (WhereIsRook(
            board, start_square, [](int i) { return (i >> 3) == 7; }, 8, is_white) ||
        WhereIsRook(
            board, start_square, [](int i) { return (i >> 3) == 0; }, -8, is_white) ||
        WhereIsRook(
            board, start_square, [](int i) { return (i & 7) == 7; }, 1, is_white) ||
        WhereIsRook(board, start_square, [](int i) { return (i & 7) == 0; }, -1, is_white))
        return true;
    return false;
}

template <typename Comp>
bool King::WhereIsRook(const ArrayBoard& board, int start_square, Comp condition, int step,
                       bool is_white) {
    int move_square = start_square;

    bool is_last_legal_move = false;
    if (condition(move_square)) is_last_legal_move = true;
    move_square += step;
    if (move_square < 0 || move_square > 63) return false;
    while (!is_last_legal_move) {
        if (condition(move_square)) is_last_legal_move = true;
        if (move_square < 0 || move_square > 63) return false;
        Figures current_figure = board[move_square];
        if (current_figure == Figures::kNone) {
            move_square += step;
            continue;
        }
        if (is_white) {
            if (current_figure == Figures::kWhiteKing) {
                move_square += step;
                continue;
            }
        } else {
            if (current_figure == Figures::kBlackKing) {
                move_square += step;
                continue;
            }
        }

        if ((Config::FigureToSide(current_figure) != is_white) &&
            (current_figure == Figures::kWhiteRook || current_figure == Figures::kBlackRook ||
             current_figure == Figures::kWhiteQueen || current_figure == Figures::kBlackQueen))
            return true;
        else
            return false;
        move_square += step;
    }
    return false;
}

bool King::IsEnemyKingClose(const ArrayBoard& board, int start_square, bool is_white) {
    int offsets[] = {-9, -8, -7, -1, 1, 7, 8, 9};
    int move_square = start_square;

    for (int offset : offsets) {
        int index = move_square + offset;

        if (index < 0 || index > 63) continue;

        // did we teleport?
        // if diff > 2, we wrapped around the board edges.
        if (std::abs((move_square & 7) - (index & 7)) > 2)
            continue;  // not sure this right check or not, need to test !!!

        Figures current_figure = board[index];

        Figures enemy_king = is_white ? Figures::kBlackKing : Figures::kWhiteKing;
        if (current_figure != Figures::kNone && board[index] == enemy_king) return true;
    }
    return false;
}

void King::GetKingMoves(MoveMap& map, const ArrayBoard& board,
                        const std::array<bool, 6>& is_in_start_pos_board, int start_square,
                        bool is_white_pov, bool is_white) {
    int offsets[] = {-9, -8, -7, -1, 1, 7, 8, 9};
    int move_square = start_square;

    for (int offset : offsets) {
        int index = move_square + offset;

        if (index < 0 || index > 63) continue;

        // did we teleport?
        // if diff > 2, we wrapped around the board edges.
        if (std::abs((move_square & 7) - (index & 7)) > 2) continue;

        Figures current_figure = board[index];
        if (current_figure != Figures::kNone &&
            Config::FigureToSide(current_figure) == is_white)
            continue;
        bool can_move = IsKingUnderAttack(board, index, is_white_pov, is_white);
        if (current_figure == Figures::kNone) {
            if (!can_move) map.emplace(index, MoveTypes::kMoveToEmptySquare);
            continue;
        } else if (Config::FigureToSide(current_figure) != is_white && !can_move) {
            map.emplace(index, MoveTypes::kCapture);
        }
    }

    Figures rook = (is_white) ? Figures::kWhiteRook : Figures::kBlackRook;
    bool is_king_start_pos = is_white ? is_in_start_pos_board[0] : is_in_start_pos_board[1];
    if (is_white_pov) {
        bool is_rook_start_pos = is_white
                                     ? is_in_start_pos_board[3]
                                     : is_in_start_pos_board[5];  // RightBottomRook or RightTopRook
        if (is_king_start_pos && board[(start_square >> 3) * 8 + 7] == rook) {
            if (board[start_square + 1] == Figures::kNone &&
                board[start_square + 2] == Figures::kNone) {
                if (is_rook_start_pos &&  // is_in_start_pos_board[(start_square >> 3) * 8 + 7]
                    !IsKingUnderAttack(board, start_square, is_white_pov, is_white) &&
                    !IsKingUnderAttack(board, start_square + 1, is_white_pov, is_white) &&
                    !IsKingUnderAttack(board, start_square + 2, is_white_pov, is_white)) {
                    map.emplace(start_square + 2, MoveTypes::kShortCastling);
                }
            }
        }
        is_rook_start_pos = is_white ? is_in_start_pos_board[2]
                                     : is_in_start_pos_board[4];  // LeftBottomRook or LeftTopRook
        if (is_king_start_pos && board[(start_square >> 3) * 8] == rook) {
            if (board[start_square - 1] == Figures::kNone &&
                board[start_square - 2] == Figures::kNone &&
                board[start_square - 3] == Figures::kNone) {
                if (is_rook_start_pos &&  // is_in_start_pos_board[(start_square >> 3) * 8]
                    !IsKingUnderAttack(board, start_square, is_white_pov,
                                       is_white)  // not sure about + 7 !!!
                    && !IsKingUnderAttack(board, start_square - 1, is_white_pov, is_white) &&
                    !IsKingUnderAttack(board, start_square - 2, is_white_pov, is_white) &&
                    !IsKingUnderAttack(board, start_square - 3, is_white_pov, is_white)) {
                    map.emplace(start_square - 2, MoveTypes::kLongCastling);
                }
            }
        }
    } else {
        bool is_rook_start_pos = !is_white
                                     ? is_in_start_pos_board[2]
                                     : is_in_start_pos_board[4];  // LeftBottomRook or LeftTopRook
        if (is_king_start_pos && board[(start_square >> 3) * 8] == rook) {
            if (board[start_square - 1] == Figures::kNone &&
                board[start_square - 2] == Figures::kNone) {
                if (is_rook_start_pos &&  // is_in_start_pos_board[(start_square >> 3) * 8]
                    !IsKingUnderAttack(board, start_square, is_white_pov, is_white) &&
                    !IsKingUnderAttack(board, start_square - 1, is_white_pov, is_white) &&
                    !IsKingUnderAttack(board, start_square - 2, is_white_pov, is_white)) {
                    map.emplace(start_square - 2, MoveTypes::kShortCastling);
                }
            }
        }
        is_rook_start_pos = !is_white
                                ? is_in_start_pos_board[3]
                                : is_in_start_pos_board[5];  // RightBottomRook or RightTopRook
        if (is_king_start_pos && board[(start_square >> 3) * 8 + 7] == rook) {
            if (board[start_square + 1] == Figures::kNone &&
                board[start_square + 2] == Figures::kNone &&
                board[start_square + 3] == Figures::kNone) {
                if (is_rook_start_pos &&  // is_in_start_pos_board[(start_square >> 3) * 8 + 7]
                    !IsKingUnderAttack(board, start_square, is_white_pov,
                                       is_white)  // not sure about + 7 !!!
                    && !IsKingUnderAttack(board, start_square + 1, is_white_pov, is_white) &&
                    !IsKingUnderAttack(board, start_square + 2, is_white_pov, is_white) &&
                    !IsKingUnderAttack(board, start_square + 3, is_white_pov, is_white)) {
                    map.emplace(start_square + 2, MoveTypes::kLongCastling);
                }
            }
        }
    }
}
