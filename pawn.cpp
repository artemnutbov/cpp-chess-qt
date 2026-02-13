#include "pawn.h"

void Pawn::GetPawnMoves(MoveMap& map, const ArrayBoard& board,
                        const std::array<bool, 64>& is_in_start_pos_board, int start_square,
                        int en_passant_pawn_square, bool is_white_pov, bool is_en_passant) {
    int move_square = start_square;
    bool is_white = Config::FigureToSide(board[move_square]);
    bool is_go_up = is_white ? is_white_pov : !is_white_pov;

    is_go_up ? move_square -= 8 : move_square += 8;
    if (board[move_square] == Figures::kNone) {
        if (move_square >> 3 == 0 || move_square >> 3 == 7)
            map.emplace(move_square, MoveTypes::kPromoteToEmptySquare);
        else {
            map.emplace(move_square, MoveTypes::kMoveToEmptySquare);
            is_go_up ? move_square -= 8 : move_square += 8;
            if (is_in_start_pos_board[start_square])
                map.emplace(move_square, MoveTypes::kMoveToEmptySquare);
            is_go_up ? move_square += 8 : move_square -= 8;
        }
    }

    bool is_last_legal_move = false;
    if ((move_square & 7) == 7) is_last_legal_move = true;
    ++move_square;

    if (board[move_square] != Figures::kNone && !is_last_legal_move) {
        if (Config::FigureToSide(board[move_square]) != is_white) {
            if (move_square >> 3 == 0 || move_square >> 3 == 7)
                map.emplace(move_square, MoveTypes::kPromoteCapture);
            else
                map.emplace(move_square, MoveTypes::kCapture);
        }
    } else if (is_en_passant && en_passant_pawn_square == start_square + 1) {
        map.emplace(move_square, MoveTypes::kEnPassant);
    }

    is_last_legal_move = false;
    --move_square;
    if ((move_square & 7) == 0) is_last_legal_move = true;
    --move_square;

    if (board[move_square] != Figures::kNone && !is_last_legal_move) {
        if (Config::FigureToSide(board[move_square]) != is_white) {
            if (move_square >> 3 == 0 || move_square >> 3 == 7)
                map.emplace(move_square, MoveTypes::kPromoteCapture);
            else
                map.emplace(move_square, MoveTypes::kCapture);
        }
    } else if (is_en_passant && en_passant_pawn_square == start_square - 1) {
        map.emplace(move_square, MoveTypes::kEnPassant);
    }
}
