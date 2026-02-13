#include "king.h"


bool King::is_king_under_attack(const ArrayBoard& board, int start_square,bool is_white_pov, bool is_white) { // need to rewrite bcs it is awful to take 2 int instead of  std::pair
    if(is_attacked_by_knight(board, start_square, is_white) || is_attacked_by_bishop(board, start_square, is_white)
        || is_attacked_by_rook(board, start_square, is_white) || is_attacked_by_pawn(board, start_square, is_white_pov, is_white) || is_enemy_king_close(board,  start_square, is_white))
        return true;
    return false;
}

bool King::is_attacked_by_pawn(const ArrayBoard& board,int start_square, bool is_white_pov, bool is_white) {
    bool is_go_up = is_white ?  is_white_pov : !is_white_pov;


    is_go_up ? start_square-=8 : start_square+=8;


    bool is_last_legal_move = false;
    if((start_square & 7) == 7)
        is_last_legal_move = true;
    ++start_square;
    if(board[start_square] != Figures::none && !is_last_legal_move ) {
        if(BasicFigure::figure_to_side(board[start_square]) != is_white
            && ( board[start_square] == Figures::white_pawn ||  board[start_square] == Figures::black_pawn)) {
            return true;
        }
    }

    is_last_legal_move = false;
    --start_square;
    if((start_square & 7) == 0)
        is_last_legal_move = true;
    --start_square;

    if(board[start_square] != Figures::none && !is_last_legal_move  ) {
        if(BasicFigure::figure_to_side(board[start_square]) != is_white
            && ( board[start_square] == Figures::white_pawn ||  board[start_square] == Figures::black_pawn)) {
            return true;
        }
    }


    return false;
}


bool King::is_attacked_by_knight(const ArrayBoard& board,int start_square, bool is_white) {
    int offsets[] = {-17, -15, -10, -6, 6, 10, 15, 17};
    int move_square = start_square;
    for (int offset : offsets) {
        int index = move_square + offset;

        if (index < 0 || index > 63) continue;

        // did we teleport?
        // if diff > 2, we wrapped around the board edges.
        if (std::abs((move_square & 7) - (index & 7)) > 2) continue;

        Figures current_figure = board[index];
        if(current_figure == Figures::none )
            continue;
        else if((BasicFigure::figure_to_side(current_figure) != is_white)
                 && (current_figure == Figures::white_knight || current_figure == Figures::black_knight) ) {
            return true;
        }
    }
    return false;

}


bool King::is_attacked_by_bishop(const ArrayBoard& board,int start_square, bool is_white) {

    if(where_is_bishop(board, start_square, 9, is_white) || where_is_bishop(board, start_square, 7, is_white)
        || where_is_bishop(board, start_square, -9, is_white) || where_is_bishop(board, start_square, -7, is_white))
        return true;
    return false;
}


bool King::where_is_bishop(const ArrayBoard& board, int start_square, int step, bool is_white) {
    int move_square = start_square;

    bool is_last_legal_move = false;
    if ((step == 9 || step == -7) && (move_square & 7) == 7)
        is_last_legal_move = true;
    if ((step == 7 || step == -9) && (move_square & 7) == 0)
        is_last_legal_move = true;
    move_square+=step;
    if (move_square < 0 || move_square > 63) return false;
    while(!is_last_legal_move) {
        if ((step == 9 || step == -7) && (move_square & 7) == 7)
            is_last_legal_move = true;
        if ((step == 7 || step == -9) && (move_square & 7) == 0)
            is_last_legal_move = true;
        if (move_square < 0 || move_square > 63) return false;
        Figures current_figure = board[move_square];
        if(current_figure == Figures::none ){

            move_square+=step;
            continue;
        }
        if(is_white) {
            if(current_figure == Figures::white_king) {
                move_square+=step;
                continue;
            }
        }
        else{
            if(current_figure == Figures::black_king) {
                move_square+=step;
                continue;
            }
        }
        if((BasicFigure::figure_to_side(current_figure) != is_white)
                 && (current_figure == Figures::white_bishop || current_figure == Figures::black_bishop
                       || current_figure == Figures::white_queen || current_figure == Figures::black_queen)) {
            return true;
        }
        else
            return false;

        move_square+=step;
    }
    return false;
}


bool King::is_attacked_by_rook(const ArrayBoard& board,int start_square,  bool is_white) {
    if( where_is_rook(board, start_square, [](int i) { return (i >> 3) == 7; }, 8, is_white) || where_is_rook(board, start_square, [](int i) { return (i >> 3) == 0; }, -8, is_white)
        || where_is_rook(board, start_square, [](int i) { return (i & 7) == 7; }, 1, is_white) || where_is_rook(board, start_square, [](int i) { return (i & 7) == 0; }, -1, is_white))
        return true;
    return false;
}


template <typename Comp>
bool King::where_is_rook(const ArrayBoard& board,int start_square, Comp condition, int step, bool is_white) {
    int move_square = start_square;


    bool is_last_legal_move = false;
    if(condition(move_square))
        is_last_legal_move = true;
    move_square+=step;
    if (move_square < 0 || move_square > 63) return false;
    while(!is_last_legal_move) {
        if(condition(move_square))
            is_last_legal_move = true;
        if (move_square < 0 || move_square > 63) return false;
        Figures current_figure = board[move_square];
        if(current_figure == Figures::none ){
            move_square+=step;
            continue;
        }
        if(is_white) {
            if(current_figure == Figures::white_king) {
                move_square+=step;
                continue;
            }
        }
        else{
            if(current_figure == Figures::black_king) {
                move_square+=step;
                continue;
            }
        }

        if((BasicFigure::figure_to_side(current_figure) != is_white)
                 && (current_figure == Figures::white_rook || current_figure == Figures::black_rook
                       || current_figure == Figures::white_queen || current_figure == Figures::black_queen))
            return true;
        else
            return false;
        move_square+=step;
    }
    return false;


}

bool King::is_enemy_king_close(const ArrayBoard& board, int start_square,  bool is_white) {
    int offsets[] = {-9, -8, -7, -1, 1, 7, 8, 9};
    int move_square = start_square;

    for (int offset : offsets) {
        int index = move_square + offset;

        if (index < 0 || index > 63) continue;

        // did we teleport?
        // if diff > 2, we wrapped around the board edges.
        if (std::abs((move_square & 7) - (index & 7)) > 2) continue; // not sure this right check or not, need to test !!!

        Figures current_figure = board[index];

        Figures enemy_king = is_white ? Figures::black_king : Figures::white_king;
        if(current_figure != Figures::none && board[index] == enemy_king )
            return true;
    }
    return false;
}

void King::get_king_moves(MoveMap& map, const ArrayBoard& board, const std::array<bool,64>& is_in_start_pos_board, int start_square, bool is_white_pov, bool is_white) {
    int offsets[] = {-9, -8, -7, -1, 1, 7, 8, 9};
    int move_square = start_square;

    for (int offset : offsets) {
        int index = move_square + offset;

        if (index < 0 || index > 63) continue;

        // did we teleport?
        // if diff > 2, we wrapped around the board edges.
        if (std::abs((move_square & 7) - (index & 7)) > 2) continue;

        Figures current_figure = board[index];
        if(current_figure != Figures::none && BasicFigure::figure_to_side(current_figure) == is_white)
            continue;
        bool can_move = is_king_under_attack(board, index, is_white_pov, is_white);
        if(current_figure == Figures::none) {
            if(!can_move)
                map.emplace(index, Move_types::move_to_empty_square);
            continue;
        }
        else if(BasicFigure::figure_to_side(current_figure) != is_white && !can_move) {
            map.emplace(index, Move_types::capture);
        }
    }

    Figures rook = (is_white) ? Figures::white_rook : Figures::black_rook;

    if(is_white_pov) {
        if(is_in_start_pos_board[start_square] && board[(start_square >> 3) * 8 + 7] == rook) {
            if(board[start_square + 1] ==  Figures::none && board[start_square + 2] ==  Figures::none) {
                if(is_in_start_pos_board[(start_square >> 3) * 8 + 7] && !is_king_under_attack(board, start_square, is_white_pov, is_white)
                    &&  !is_king_under_attack(board, start_square + 1, is_white_pov, is_white) && !is_king_under_attack(board, start_square + 2, is_white_pov, is_white)) {
                    map.emplace(start_square + 2, Move_types::short_castling);
                }
            }
        }
        if(is_in_start_pos_board[start_square] && board[(start_square >> 3) * 8 ] == rook) {
            if(board[start_square - 1] ==  Figures::none && board[start_square - 2] == Figures::none && board[start_square - 3] ==  Figures::none) {
                if(is_in_start_pos_board[(start_square >> 3) * 8 ] && !is_king_under_attack(board, start_square, is_white_pov, is_white) // not sure about + 7 !!!
                    &&  !is_king_under_attack(board,start_square - 1,is_white_pov, is_white) && !is_king_under_attack(board, start_square - 2, is_white_pov, is_white)
                    && !is_king_under_attack(board, start_square - 3, is_white_pov, is_white)) {
                    map.emplace(start_square - 2, Move_types::long_castling);
                }
            }
        }
    }
    else {
        if(is_in_start_pos_board[start_square] && board[(start_square >> 3) * 8] == rook) {
            if(board[start_square - 1] ==  Figures::none && board[start_square - 2] ==  Figures::none) {
                if(is_in_start_pos_board[(start_square >> 3) * 8]  && !is_king_under_attack(board, start_square, is_white_pov, is_white)
                    &&  !is_king_under_attack(board, start_square - 1, is_white_pov, is_white) && !is_king_under_attack(board, start_square - 2, is_white_pov, is_white)) {
                    map.emplace(start_square - 2, Move_types::short_castling);
                }
            }
        }

        if(is_in_start_pos_board[start_square] && board[(start_square >> 3) * 8 + 7] == rook) {
            if(board[start_square + 1] ==  Figures::none && board[start_square + 2] == Figures::none && board[start_square + 3] ==  Figures::none) {
                if(is_in_start_pos_board[(start_square >> 3) * 8 + 7] && !is_king_under_attack(board, start_square, is_white_pov, is_white) // not sure about + 7 !!!
                    &&  !is_king_under_attack(board,start_square + 1,is_white_pov, is_white) && !is_king_under_attack(board, start_square + 2, is_white_pov, is_white)
                    && !is_king_under_attack(board, start_square + 3, is_white_pov, is_white)) {
                    map.emplace(start_square + 2, Move_types::long_castling);
                }
            }
        }
    }

}
