#ifndef QUEEN_H
#define QUEEN_H
#include "config.h"

class Queen {
public:
    static void GetQueenMoves(MoveMap&, const ArrayBoard&, int);
};

#endif  // QUEEN_H
