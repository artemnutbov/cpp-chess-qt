#ifndef KING_H
#define KING_H
#include "BasicFigure.h"
class King {
public:

    static void get_king_moves(MoveMap& ,const ArrayBoard&, const std::array<bool,64>& ,int ,bool ,bool ) ;
    static bool is_king_under_attack(const ArrayBoard& , int,  bool,  bool);
private:

    static bool is_attacked_by_pawn(const ArrayBoard& ,int, bool, bool);
    static bool is_attacked_by_knight(const ArrayBoard& ,int, bool);
    static bool is_attacked_by_bishop(const ArrayBoard&, int, bool);
    //bool where_is_knight(const ArrayBoard&, std::pair<int,int>, int, int);

    static bool where_is_pawn(const ArrayBoard&, int, bool, bool);
    static bool where_is_bishop(const ArrayBoard&, int, int, bool);

    static bool is_attacked_by_rook(const ArrayBoard&, int, bool);
    template <typename Comp>
    static bool where_is_rook(const ArrayBoard&, int, Comp,int, bool);

    static bool is_enemy_king_close(const ArrayBoard&, int, bool);
private:

    static void what_castling(MoveMap& ,const ArrayBoard&, int, int);
};

#endif // KING_H
