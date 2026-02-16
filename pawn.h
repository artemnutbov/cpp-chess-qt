#ifndef PAWN_H
#define PAWN_H
#include "config.h"

class Pawn {
public:
    static void GetPawnMoves(MoveMap&, const ArrayBoard&, int, int, bool, bool);
};

#endif  // PAWN_H
