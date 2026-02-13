#include "queen.h"
#include "bishop.h"
#include "rook.h"

void Queen::get_queen_moves(MoveMap& map, const ArrayBoard& board, int start_square ) {

        Rook::get_rook_moves(map, board, start_square);
        Bishop::get_bishop_moves(map, board, start_square);
}
