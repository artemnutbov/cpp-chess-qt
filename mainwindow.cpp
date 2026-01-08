#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QPainter>
#include <QDebug>
#include <QPainterPath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), x(0), y(0), start_x_pos(100), start_y_pos(70), cell_size(60)
{
    ui->setupUi(this);
    set_up_images();
    set_up();

}

QPixmap MainWindow::create_image(const char* path,bool is_white) {
    QPixmap img(path);   // or a normal file path

    QBitmap mask;
    if(is_white)
        mask = img.createMaskFromColor(Qt::black);
    else
        mask = img.createMaskFromColor(Qt::white);
    img.setMask(mask);
    img = img.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    return img;
}
void MainWindow::set_up_images() {
    images_map[Figures::white_pawn] = create_image("C:/Users/38096/Documents/my_chess_game/images/white_pawn.png",true);
    images_map[Figures::white_bishop] = create_image("C:/Users/38096/Documents/my_chess_game/images/white_bishop.png",true);
    images_map[Figures::white_knight] = create_image("C:/Users/38096/Documents/my_chess_game/images/white_knight.png",true);
    images_map[Figures::white_rook] = create_image("C:/Users/38096/Documents/my_chess_game/images/white_rook.png",true);
    images_map[Figures::white_queen] = create_image("C:/Users/38096/Documents/my_chess_game/images/white_queen.png",true);

    images_map[Figures::white_king] = create_image("C:/Users/38096/Documents/my_chess_game/images/white_king.png",true);


    images_map[Figures::black_pawn] = create_image("C:/Users/38096/Documents/my_chess_game/images/black_pawn.png",false);
    images_map[Figures::black_bishop] = create_image("C:/Users/38096/Documents/my_chess_game/images/black_bishop.png",false);
    images_map[Figures::black_knight] = create_image("C:/Users/38096/Documents/my_chess_game/images/black_knight.png",false);
    images_map[Figures::black_rook] = create_image("C:/Users/38096/Documents/my_chess_game/images/black_rook.png",false);
    images_map[Figures::black_queen] = create_image("C:/Users/38096/Documents/my_chess_game/images/black_queen.png",false);
    images_map[Figures::black_king] = create_image("C:/Users/38096/Documents/my_chess_game/images/black_king.png",false);


}
void MainWindow::set_up() {
    // coordinates_board set_up
    for(size_t i = 0; i < 8;++i) {
        for(size_t j = 0;j < 8;++j) {
            coordinates_board[i][j] = QPoint(start_x_pos+cell_size*j,start_y_pos+cell_size*i);
        }
    }
    board.set_up();


}
void MainWindow::paintEvent(QPaintEvent *)
{
    // draw board
    QPainter p(this);
    QColor my_white(235,246,208);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setPen(Qt::NoPen);
    p.setBrush(my_white);

    for(size_t i = 0; i < 4;++i) {
        for(size_t j = 0;j < 8;++j) {
            if(j % 2 == 0)
                p.drawRect(coordinates_board[i*2][j].x(),coordinates_board[i*2][j].y(), cell_size, cell_size);
            else
                p.drawRect(coordinates_board[1+i*2][j].x(),coordinates_board[1+i*2][j].y(), cell_size, cell_size);
        }
    }
    QColor my_green(115,149,82);
    p.setPen(Qt::NoPen);
    p.setBrush(my_green);

    for(size_t i = 0; i < 4;++i) {
        for(size_t j = 0;j < 8;++j) {
            if(j % 2 == 0)
                p.drawRect(coordinates_board[1+i*2][j].x(),coordinates_board[1+i*2][j].y(), cell_size, cell_size);
            else
                p.drawRect(coordinates_board[i*2][j].x(),coordinates_board[i*2][j].y(), cell_size, cell_size);
        }
    }

    if(is_first_click) { // demo of on what figure you click
        QColor my_transparent_yellow(245,246,130,160);
        p.setBrush(my_transparent_yellow);
        p.drawRect(coordinates_board[y][x].x(),coordinates_board[y][x].y(), cell_size, cell_size);


        QColor my_transparent_black(0,0,0, 20);

        int circle_radius = cell_size/6;
        int half_cell_size = cell_size/2;
        int cicle_thickness = 6;
        QPainterPath path;
        p.setBrush(my_transparent_black);
        for(const auto& it : board.all_legal_moves()) {
            QPointF center_of_cell(it.first.second*cell_size + start_x_pos + half_cell_size ,it.first.first*cell_size + half_cell_size +  start_y_pos);

            if(it.second == Move_types::capture){ // idk problem with drawing that circle same transparent color
                path.addEllipse(center_of_cell,half_cell_size,half_cell_size);
                path.addEllipse(center_of_cell,half_cell_size-cicle_thickness,half_cell_size-cicle_thickness);
                p.setBrush(my_transparent_black);
                p.fillPath(path,p.brush());

            }
            else
               p.drawEllipse(center_of_cell , circle_radius, circle_radius);
        }

    }

    for(size_t i = 0;i < 8;++i) {
        for(size_t j = 0;j < 8;++j){

            if(board.valid_index(i,j))
                p.drawPixmap(coordinates_board[i][j].x(), coordinates_board[i][j].y(),images_map[board.what_figure_index(i,j)]);
        }
    }

}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    // check for click

    QPoint pos = event->pos();
    QRect area(start_x_pos,start_y_pos,cell_size*8,cell_size*8);
    if(!area.contains(pos)) {
        return;
    }

    if(!is_first_click) {
        is_first_click = true;
        x = (pos.x() - start_x_pos) / 60;
        y = (pos.y() - start_y_pos) / 60;
        if(!board.valid_index(y,x))
            is_first_click = false;
        else {
            board.all_figure_move(x,y);
        }
    }
    else{
        int new_x_index = (pos.x() - start_x_pos) / 60;
        int new_y_index = (pos.y() - start_y_pos) / 60;

        board.action_move(x, y, new_x_index, new_y_index);

        is_first_click = false;
    }
    update();
}


MainWindow::~MainWindow()
{
    delete ui;
}
