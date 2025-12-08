#ifndef BASIC_FIGURE_CPP
#define BASIC_FIGURE_CPP
#include "Basic_figure.h"
#include <QPainter>

Basic_figure::Basic_figure(const Figures& figure,int x, int y,bool is_white)
    :figure(figure),x(x),y(y),is_white(is_white) {}

Figures Basic_figure::what_figure() const{
    return figure;
}
void Basic_figure::handle_move() {

}

void Basic_figure::draw(QPainter& p,const QPixmap& img) {
    p.drawPixmap(x, y, img);

}

#endif // BASIC_FIGURE_CPP

