#ifndef BISHOP_H
#define BISHOP_H
#include "BasicFigure.h"

class Bishop {
public:
    static void get_bishop_moves(MoveMap& , const ArrayBoard& ,int );
protected:
    static void what_to_do_whith_figure(MoveMap&, const ArrayBoard&, int, int);
};

#endif // BISHOP_H
