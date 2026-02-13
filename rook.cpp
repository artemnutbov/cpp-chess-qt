#include "rook.h"

void Rook::GetRookMoves(MoveMap& map, const ArrayBoard& board, int start_square) {
    WhatToDoWhithFigure(map, board, start_square, [](int i) { return (i >> 3) == 7; }, 8);

    WhatToDoWhithFigure(map, board, start_square, [](int i) { return (i >> 3) == 0; }, -8);

    WhatToDoWhithFigure(map, board, start_square, [](int i) { return (i & 7) == 7; }, 1);

    WhatToDoWhithFigure(
        map, board, start_square,
        [](int i) {
            return (i & 7) == 0;
            ;
        },
        -1);
}

template <typename Comp>
void Rook::WhatToDoWhithFigure(MoveMap& map, const ArrayBoard& board, int start_square,
                               Comp condition, int step) {
    int move_square = start_square;

    bool is_last_legal_move = false;

    if (condition(move_square)) is_last_legal_move = true;

    move_square += step;

    if (move_square < 0 || move_square > 63) return;  // mb don't need this if statement
    while (!is_last_legal_move) {
        if (condition(move_square)) is_last_legal_move = true;
        if (move_square < 0 || move_square > 63) return;
        Figures current_figure = board[move_square];
        if (current_figure == Figures::kNone)
            map.emplace(move_square, MoveTypes::kMoveToEmptySquare);
        else if ((Config::FigureToSide(current_figure) !=
                  Config::FigureToSide(board[start_square])) &&
                 (current_figure != Figures::kWhiteKing) &&
                 (current_figure != Figures::kBlackKing)) {
            map.emplace(move_square, MoveTypes::kCapture);
            break;
        } else
            break;

        move_square += step;
    }
}
