#ifndef QUEEN_H
#define QUEEN_H
#include "rook.h"
#include "bishop.h"

class Queen: public Rook, public  Bishop {
public:
    Queen(const Figures& figure, int x, int y, bool is_white);

    void where_to_move(MoveMap& ,const ArrayBoard& , int , int , bool ) override;
    //virtual void handle_move() override;
private:
    //void move_for_both_sides(MoveMap& , const ArrayBoard& ,int , int );

    //template <typename Comp>
    //void what_to_do_whith_figure(MoveMap& , const ArrayBoard& ,std::pair<int,int>& , Comp, bool, int&);
};

#endif // QUEEN_H
