#include "queen.h"

Queen::Queen(const Figures& figure, bool is_white)
    :Basic_figure(figure, is_white), Rook(figure, is_white), Bishop(figure, is_white) {}



void Queen::where_to_move(MoveMap& map, const ArrayBoard& board, int current_i, int current_j,bool is_white_turn_to_move) {

    if(is_white && is_white_turn_to_move) {
        Rook::move_for_both_sides(map, board, current_i, current_j);
        Bishop::move_for_both_sides(map, board, current_i, current_j);

    }
    else if(!is_white && !is_white_turn_to_move) {
        Rook::move_for_both_sides(map, board, current_i, current_j);
        Bishop::move_for_both_sides(map, board, current_i, current_j);
    }
}
