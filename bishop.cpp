#include "bishop.h"


void Bishop::get_bishop_moves(MoveMap& map,const ArrayBoard& board,int current_i) {
    what_to_do_whith_figure(map, board, current_i, -9);

    what_to_do_whith_figure(map, board, current_i, -7);

    what_to_do_whith_figure(map, board, current_i, 9);

    what_to_do_whith_figure(map, board, current_i, 7);

}


void Bishop::what_to_do_whith_figure(MoveMap& map, const ArrayBoard& board, int start_square, int step) {
    int move_square = start_square;

    bool is_last_legal_move = false;
    if ((step == 9 || step == -7) && (move_square & 7) == 7)
        is_last_legal_move = true;
    if ((step == 7 || step == -9) && (move_square & 7) == 0)
        is_last_legal_move = true;
    move_square+=step;
    if (move_square < 0 || move_square > 63) return;
    while(!is_last_legal_move) {

        if ((step == 9 || step == -7) && (move_square & 7) == 7)
            is_last_legal_move = true;
        if ((step == 7 || step == -9) && (move_square & 7) == 0)
            is_last_legal_move = true;
        if (move_square < 0 || move_square > 63) return;
        Figures current_figure = board[move_square];
        if(current_figure == Figures::none )
            map.emplace(move_square,Move_types::move_to_empty_square);
        else if((BasicFigure::figure_to_side(current_figure) != BasicFigure::figure_to_side(board[start_square]))
                 && (current_figure != Figures::white_king) && (current_figure != Figures::black_king) ) {
            map.emplace(move_square,Move_types::capture);
            break;
        }
        else
            break;

        move_square+=step;
    }
}
