#ifndef KING_H
#define KING_H
#include "Basic_figure.h"
class King:public Basic_figure {
public:
    King(const Figures& , bool );

    void where_to_move(MoveMap& ,const ArrayBoard& , int , int , bool ) override;
    bool is_king_under_attack(const ArrayBoard& , int , int);
    virtual void handle_move() override;
private:

    bool is_attacked_by_pawn(const ArrayBoard& ,int, int);
    bool is_attacked_by_knight(const ArrayBoard& ,int, int);
    bool is_attacked_by_bishop(const ArrayBoard&, int, int);
    bool where_is_knight(const ArrayBoard&, std::pair<int,int>, int, int);

    bool where_is_pawn(const ArrayBoard&, std::pair<int,int>, bool);
    template <typename Comp>
    bool where_is_bishop(const ArrayBoard&, std::pair<int,int>, const Comp&, const Comp&, bool, bool);

    bool is_attacked_by_rook(const ArrayBoard&, int, int);
    template <typename Comp>
    bool where_is_rook(const ArrayBoard&, std::pair<int,int>, Comp, bool, bool);
private:
    void move_for_both_sides(MoveMap& ,const ArrayBoard&, int, int);
    bool is_in_start_pos;

    void what_castling(MoveMap& ,const ArrayBoard&, int, int);
};

#endif // KING_H
