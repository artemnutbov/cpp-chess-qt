#include "rook.h"


void Rook::get_rook_moves(MoveMap& map,const ArrayBoard& board,int start_square) {
    what_to_do_whith_figure(map, board, start_square, [](int i) { return (i >> 3) == 7; }, 8);

    what_to_do_whith_figure(map, board, start_square, [](int i) { return (i >> 3) == 0; }, -8);

    what_to_do_whith_figure(map, board, start_square, [](int i) { return (i & 7) == 7; }, 1);

    what_to_do_whith_figure(map, board, start_square, [](int i) { return (i & 7) == 0;; }, -1);

}


template <typename Comp>
void Rook::what_to_do_whith_figure(MoveMap& map, const ArrayBoard& board,int start_square, Comp condition, int step) {
    int move_square = start_square;

    bool is_last_legal_move = false;

    if(condition(move_square))
        is_last_legal_move = true;

    move_square+=step;

    if (move_square < 0 || move_square > 63) return; // mb don't need this if statement
    while(!is_last_legal_move) {
        if(condition(move_square))
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

