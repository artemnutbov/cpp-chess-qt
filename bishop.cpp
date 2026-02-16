#include "bishop.h"

void Bishop::GetBishopMoves(MoveMap& map, const ArrayBoard& board, int start_square) {
    WhatToDoWhithFigure(map, board, start_square, -9);

    WhatToDoWhithFigure(map, board, start_square, -7);

    WhatToDoWhithFigure(map, board, start_square, 9);

    WhatToDoWhithFigure(map, board, start_square, 7);
}

void Bishop::WhatToDoWhithFigure(MoveMap& map, const ArrayBoard& board, int start_square,
                                 int step) {
    int move_square = start_square;
    bool is_last_legal_move = false;
    if ((step == 9 || step == -7) && (move_square & 7) == 7) is_last_legal_move = true;
    if ((step == 7 || step == -9) && (move_square & 7) == 0) is_last_legal_move = true;
    move_square += step;
    if (move_square < 0 || move_square > 63) return;
    while (!is_last_legal_move) {
        if ((step == 9 || step == -7) && (move_square & 7) == 7) is_last_legal_move = true;
        if ((step == 7 || step == -9) && (move_square & 7) == 0) is_last_legal_move = true;
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
