#ifndef KING_H
#define KING_H
#include "config.h"
class King {
public:
    static void GetKingMoves(MoveMap&, const ArrayBoard&, const std::array<bool, 6>&, int, bool,
                             bool);
    static bool IsKingUnderAttack(const ArrayBoard&, int, bool, bool);
    static bool IsFreeToMove(const ArrayBoard&, int, bool);

private:
    static bool IsAttackedByPawn(const ArrayBoard&, int, bool, bool);
    static bool IsAttackedByKnight(const ArrayBoard&, int, bool);
    static bool IsAttackedByBishop(const ArrayBoard&, int, bool);

    static bool WhereIsPawn(const ArrayBoard&, int, bool, bool);
    static bool WhereIsBishop(const ArrayBoard&, int, int, bool);

    static bool IsAttackedByRook(const ArrayBoard&, int, bool);
    template <typename Comp>
    static bool WhereIsRook(const ArrayBoard&, int, Comp, int, bool);

    static bool IsEnemyKingClose(const ArrayBoard&, int, bool);
};

#endif  // KING_H
