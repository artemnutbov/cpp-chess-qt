#ifndef BISHOP_H
#define BISHOP_H
#include "Basic_figure.h"

class Bishop: public virtual Basic_figure {
public:

    Bishop(const Figures& figure, int x, int y, bool is_white);

    void where_to_move(MoveMap& ,const ArrayBoard& , int , int , bool ) override;
    virtual void handle_move() override;
protected:
    void move_for_both_sides(MoveMap& , const ArrayBoard& ,int , int );

    template <typename Comp>
    void what_to_do_whith_figure(MoveMap&, const ArrayBoard&, std::pair<int,int>, const Comp&, const Comp&, bool, bool);
};

#endif // BISHOP_H
