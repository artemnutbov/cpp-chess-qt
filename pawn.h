#ifndef PAWN_H
#define PAWN_H
#include "Basic_figure.h"

class Pawn: public Basic_figure {
private:
    bool is_in_start_pos;
public:

    Pawn(const Figures&, bool is_white);
    void where_to_move(MoveMap& map,const ArrayBoard& board,
                       int current_i, int current_j, bool is_white_turn_to_move) override;
    virtual void handle_move() override;
};


#endif // PAWN_H
