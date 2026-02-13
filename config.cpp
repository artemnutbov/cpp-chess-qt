#ifndef Config_CPP
#define Config_CPP
#include "config.h"

bool Config::FigureToSide(Figures figure) {
    if (figure == Figures::kWhitePawn || figure == Figures::kWhiteRook ||
        figure == Figures::kWhiteKnight || figure == Figures::kWhiteBishop ||
        figure == Figures::kWhiteQueen || figure == Figures::kWhiteKing)
        return true;
    return false;
}
#endif  // Config_CPP
