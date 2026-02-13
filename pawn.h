#ifndef PAWN_H
#define PAWN_H
#include "BasicFigure.h"

class Pawn {
public:

    static void get_pawn_moves(MoveMap& ,const ArrayBoard& ,const std::array<bool,64>&, int ,int ,bool ,bool);
};


#endif // PAWN_H
