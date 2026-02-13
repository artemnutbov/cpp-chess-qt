#ifndef BasicFigure_H
#define BasicFigure_H

#include <boost/unordered/unordered_flat_map.hpp>
#include <cmath>
enum class Move_types {
    move_to_empty_square, short_castling, long_castling, capture, promote_to_empty_square, promote_and_capture, en_passant
};

using IndexPair = std::pair<int,Move_types>;
using MoveMap = boost::unordered_flat_map<int, Move_types>;

enum class Figures {
    white_pawn,white_king,white_knight,white_queen,white_rook,white_bishop,
    black_pawn,black_king,black_knight,black_queen,black_rook,black_bishop,none
};


class BasicFigure {

public:
    static bool figure_to_side(Figures);
};

using ArrayBoard = std::array<Figures ,64>;
#endif // BasicFigure_H

