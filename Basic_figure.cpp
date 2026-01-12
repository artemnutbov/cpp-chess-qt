#ifndef BASIC_FIGURE_CPP
#define BASIC_FIGURE_CPP
#include "Basic_figure.h"

Basic_figure::Basic_figure(const Figures& figure, bool is_white)
    :figure(figure), is_white(is_white) {}

Figures Basic_figure::what_figure() const{
    return figure;
}
void Basic_figure::handle_move() {

}

bool Basic_figure::is_white_figure() const{
    return is_white;
}

#endif // BASIC_FIGURE_CPP

