#ifndef ROOK_H
#define ROOK_H
#include "config.h"
class Rook {
public:
    static void GetRookMoves(MoveMap&, const ArrayBoard&, int);
    template <typename Comp>
    static void WhatToDoWhithFigure(MoveMap&, const ArrayBoard&, int, Comp, int);
};

#endif  // ROOK_H
