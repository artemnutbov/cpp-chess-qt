#ifndef ROOK_H
#define ROOK_H
#include "BasicFigure.h"
class Rook {
public:
    static void get_rook_moves(MoveMap& , const ArrayBoard& ,int );
    template <typename Comp>
    static void what_to_do_whith_figure(MoveMap& , const ArrayBoard& ,int , Comp, int);
};

#endif // ROOK_H
