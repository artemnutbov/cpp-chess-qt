#ifndef BasicFigure_CPP
#define BasicFigure_CPP
#include "BasicFigure.h"


bool BasicFigure::figure_to_side(Figures figure) {
    if(figure == Figures::white_pawn || figure == Figures::white_rook || figure == Figures::white_knight
        || figure == Figures::white_bishop || figure == Figures::white_queen || figure == Figures::white_king)
        return true;
    return false;
}
#endif // BasicFigure_CPP

