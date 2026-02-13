#include "knight.h"

void Knight::get_knight_moves(MoveMap& map,const ArrayBoard& board,int start_square) {
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
            map.emplace(index,Move_types::move_to_empty_square);
        else if((BasicFigure::figure_to_side(current_figure) != BasicFigure::figure_to_side(board[start_square]))
                 && (current_figure != Figures::white_king) && (current_figure != Figures::black_king) ) {
            map.emplace(index,Move_types::capture);
        }
    }
}
