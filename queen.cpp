#include "queen.h"

#include "bishop.h"
#include "rook.h"

void Queen::GetQueenMoves(MoveMap& map, const ArrayBoard& board, int start_square) {
    Rook::GetRookMoves(map, board, start_square);
    Bishop::GetBishopMoves(map, board, start_square);
}
