#ifndef BISHOP_H
#define BISHOP_H
#include "config.h"

class Bishop {
public:
    static void GetBishopMoves(MoveMap&, const ArrayBoard&, int);

protected:
    static void WhatToDoWhithFigure(MoveMap&, const ArrayBoard&, int, int);
};

#endif  // BISHOP_H
