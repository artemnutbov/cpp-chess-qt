#include "queen.h"

Queen::Queen(const Figures& figure, bool is_white)
    :Basic_figure(figure, is_white), Rook(figure, is_white), Bishop(figure, is_white) {}



void Queen::where_to_move(MoveMap& map, const ArrayBoard& board, int current_i, int current_j,bool ) {

        Rook::move_for_both_sides(map, board, current_i, current_j);
        Bishop::move_for_both_sides(map, board, current_i, current_j);
}
