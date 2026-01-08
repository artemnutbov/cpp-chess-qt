#ifndef QUEEN_H
#define QUEEN_H
#include "rook.h"
#include "bishop.h"

class Queen: public Rook, public  Bishop {
public:
    Queen(const Figures& ,bool );

    void where_to_move(MoveMap& ,const ArrayBoard& , int , int , bool ) override;
private:

};

#endif // QUEEN_H
