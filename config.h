#ifndef Config_H
#define Config_H

#include <boost/unordered/unordered_flat_map.hpp>
#include <cmath>
enum class MoveTypes {
    kMoveToEmptySquare,
    kShortCastling,
    kLongCastling,
    kCapture,
    kPromoteToEmptySquare,
    kPromoteCapture,
    kEnPassant
};

using IndexPair = std::pair<int, MoveTypes>;
using MoveMap = boost::unordered_flat_map<int, MoveTypes>;

enum class Figures {
    kWhitePawn,
    kWhiteKing,
    kWhiteKnight,
    kWhiteQueen,
    kWhiteRook,
    kWhiteBishop,
    kBlackPawn,
    kBlackKing,
    kBlackKnight,
    kBlackQueen,
    kBlackRook,
    kBlackBishop,
    kNone
};

class Config {
public:
    static bool FigureToSide(Figures);
};

using ArrayBoard = std::array<Figures, 64>;
#endif  // Config_H
