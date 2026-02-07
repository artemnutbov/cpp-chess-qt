#ifndef PAWN_H
#define PAWN_H
#include "Basic_figure.h"

class Pawn: public Basic_figure {
private:
    bool is_in_start_pos;
    void move_for_both_sides(MoveMap& , const ArrayBoard& ,std::pair<int,int> ,bool );
public:

    Pawn(const Figures&, bool);
    void where_to_move(MoveMap& ,const ArrayBoard&, int , int , bool ) override;
    virtual void handle_move() override;
};


#endif // PAWN_H
