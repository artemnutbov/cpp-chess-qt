#include "pawn.h"

void Pawn::get_pawn_moves(MoveMap& map, const ArrayBoard& board, const std::array<bool,64>& is_in_start_pos_board,
                          int start_square,int en_passant_pawn_square, bool is_white_pov,bool is_en_passant) {
    int move_square = start_square;
    bool is_white = BasicFigure::figure_to_side(board[move_square]);
    bool is_go_up = is_white ?  is_white_pov : !is_white_pov;

    is_go_up ? move_square-=8 : move_square+=8;
    if(board[move_square] == Figures::none) {
        if(move_square >> 3 == 0 || move_square >> 3 == 7)
            map.emplace(move_square, Move_types::promote_to_empty_square);
        else {
            map.emplace(move_square, Move_types::move_to_empty_square);
            is_go_up ? move_square-=8 : move_square+=8;
            if(is_in_start_pos_board[start_square])
                map.emplace(move_square, Move_types::move_to_empty_square);
            is_go_up ? move_square+=8 : move_square-=8;
        }
    }

    bool is_last_legal_move = false;
    if((move_square & 7) == 7)
        is_last_legal_move = true;
    ++move_square;

    if(board[move_square] != Figures::none && !is_last_legal_move ) {
        if(BasicFigure::figure_to_side(board[move_square]) != is_white ) {
            if(move_square >> 3 == 0 || move_square >> 3 == 7)
                map.emplace(move_square, Move_types::promote_and_capture);
            else
                map.emplace(move_square, Move_types::capture);
        }
    }
    else if(is_en_passant && en_passant_pawn_square == start_square + 1 ) {
        map.emplace(move_square, Move_types::en_passant);
    }

    is_last_legal_move = false;
    --move_square;
    if((move_square & 7) == 0)
        is_last_legal_move = true;
    --move_square;

    if(board[move_square] != Figures::none && !is_last_legal_move  ) {
        if(BasicFigure::figure_to_side(board[move_square]) != is_white ){
            if(move_square >> 3 == 0 || move_square >> 3 == 7)
                map.emplace(move_square, Move_types::promote_and_capture);
            else
                map.emplace(move_square, Move_types::capture);
        }
    }
    else if(is_en_passant && en_passant_pawn_square == start_square - 1 ) {
        map.emplace(move_square, Move_types::en_passant);
    }
}

