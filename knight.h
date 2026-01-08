#ifndef KNIGHT_H
#define KNIGHT_H
#include "Basic_figure.h"

class Knight: public  Basic_figure {
public:
    Knight(const Figures& ,bool );

    void where_to_move(MoveMap& ,const ArrayBoard& , int , int , bool ) override;

private:
    void move_for_both_sides(MoveMap& , const ArrayBoard& ,int , int );

    void what_to_do_whith_figure(MoveMap&, const ArrayBoard&, std::pair<int,int>, int, int);
};

#endif // KNIGHT_H
