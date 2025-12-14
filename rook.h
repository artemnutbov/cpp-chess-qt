#ifndef ROOK_H
#define ROOK_H
#include "Basic_figure.h"
class Rook: public virtual Basic_figure {
public:

    Rook(const Figures& figure, int x, int y, bool is_white);

    void where_to_move(MoveMap& ,const ArrayBoard& , int , int , bool ) override;
    virtual void handle_move() override;
    bool is_in_start_position()const;
protected:
    void move_for_both_sides(MoveMap& , const ArrayBoard& ,int , int );

    template <typename Comp>
    void what_to_do_whith_figure(MoveMap& , const ArrayBoard& , std::pair<int,int> , Comp, bool, bool);
private:
    bool is_in_start_pos;
};

#endif // ROOK_H
