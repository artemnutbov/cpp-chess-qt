#include "knight.h"

void Knight::GetKnightMoves(MoveMap& map, const ArrayBoard& board, int start_square) {
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
            map.emplace(index, MoveTypes::kMoveToEmptySquare);
        else if ((Config::FigureToSide(current_figure) !=
                  Config::FigureToSide(board[start_square])) &&
                 (current_figure != Figures::kWhiteKing) &&
                 (current_figure != Figures::kBlackKing)) {
            map.emplace(index, MoveTypes::kCapture);
        }
    }
}
