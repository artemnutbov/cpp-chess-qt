#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QPainter>
#include <QDebug>
#include <QPoint>
#include "pawn.h"
#include <QPainterPath>
#include "queen.h"
#include "knight.h"
#include "king.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), x(0), y(0), start_x_pos(100), start_y_pos(70), cell_size(60), black_king_index(7, 3), white_king_index(0, 3)
{
    ui->setupUi(this);
    set_up_images();
    for(size_t i = 0;i < 8;++i) {
        for(size_t j = 0;j < 8;++j){
            board[i][j] = nullptr;
        }
    }
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


    board[0][2] = std::make_unique<Bishop>(Figures::white_bishop,start_x_pos+2*cell_size,start_y_pos,true);
    board[0][5] = std::make_unique<Bishop>(Figures::white_bishop,start_x_pos+5*cell_size,start_y_pos,true);

    board[0][1] = std::make_unique<Knight>(Figures::white_knight,start_x_pos+1*cell_size,start_y_pos,true);
    board[0][6] = std::make_unique<Knight>(Figures::white_knight,start_x_pos+6*cell_size,start_y_pos,true);
    board[0][0] = std::make_unique<Rook>(Figures::white_rook,start_x_pos,start_y_pos,true);
    board[0][7] = std::make_unique<Rook>(Figures::white_rook,start_x_pos+7*cell_size,start_y_pos,true);
    board[0][3] = std::make_unique<King>(Figures::white_king,start_x_pos+3*cell_size,start_y_pos,true);
    board[0][4] = std::make_unique<Queen>(Figures::white_queen,start_x_pos+4*cell_size,start_y_pos,true);

    board[7][0] = std::make_unique<Rook>(Figures::black_rook,start_x_pos,start_y_pos+ cell_size * 7,false);
    board[7][7] = std::make_unique<Rook>(Figures::black_rook,start_x_pos+7*cell_size,start_y_pos+ cell_size * 7,false);



    board[7][2] = std::make_unique<Bishop>(Figures::black_bishop,start_x_pos+2*cell_size,start_y_pos + cell_size * 7,false);
    board[7][5] = std::make_unique<Bishop>(Figures::black_bishop,start_x_pos+5*cell_size,start_y_pos + cell_size * 7,false);

    board[7][1] = std::make_unique<Knight>(Figures::black_knight,start_x_pos+1*cell_size,start_y_pos + cell_size * 7,false);
    board[7][6] = std::make_unique<Knight>(Figures::black_knight,start_x_pos+6*cell_size,start_y_pos + cell_size * 7,false);
    board[7][3] = std::make_unique<King>(Figures::black_king,start_x_pos+3*cell_size,start_y_pos + cell_size * 7,false);
    board[7][4] = std::make_unique<Queen>(Figures::black_queen,start_x_pos+4*cell_size,start_y_pos + cell_size * 7,false);

    for(size_t i = 0;i<8;++i) {
        board[1][i] = std::make_unique<Pawn>(Figures::white_pawn,start_x_pos+i*cell_size,start_y_pos+ cell_size,true);
        board[6][i] = std::make_unique<Pawn>(Figures::black_pawn,start_x_pos+i*cell_size,start_y_pos + cell_size * 6,false);
    }

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
        for(const auto& it : index_pair_map) {
            QPointF center_of_cell(it.first.second*cell_size + start_x_pos + half_cell_size ,it.first.first*cell_size + half_cell_size +  start_y_pos);

            if(it.second){ // idk problem with drawing that circle same transparent color
                path.addEllipse(center_of_cell,half_cell_size,half_cell_size);
                path.addEllipse(center_of_cell,half_cell_size-cicle_thickness,half_cell_size-cicle_thickness);
                p.setBrush(my_transparent_black);
                p.fillPath(path,p.brush());

            }
            else
               p.drawEllipse(center_of_cell , circle_radius, circle_radius);
            //p.drawEllipse(center_of_cell , half_cell_size, half_cell_size);
        }

    }

    for(size_t i = 0;i < 8;++i) {
        for(size_t j = 0;j < 8;++j){
            if(board[i][j]) {
                board[i][j]->draw(p,images_map[board[i][j]->what_figure()]);
            }
        }
    }

    // if(is_first_click) { // hint how figures move
    //     QColor my_transparent_black(0,0,0, 40);
    //     p.setBrush(my_transparent_black);
    //     int circle_radius = cell_size/6;
    //     int half_cell_size = cell_size/2;
    //     int cicle_thickness = 6;
    //     QPainterPath path;

    //     for(const auto& it : index_pair_map) {
    //         QPointF center_of_cell(it.first.second*cell_size + start_x_pos + half_cell_size ,it.first.first*cell_size + half_cell_size +  start_y_pos);
    //         if(it.second){
    //             path.addEllipse(center_of_cell,half_cell_size,half_cell_size);
    //             path.addEllipse(center_of_cell,half_cell_size-cicle_thickness,half_cell_size-cicle_thickness);
    //             path.setFillRule(Qt::OddEvenFill);
    //             p.drawPath(path);
    //         }
    //         else
    //             p.drawEllipse(center_of_cell , circle_radius, circle_radius);
    //     }

    //     //p.setRenderHint(QPainter::Antialiasing, true); // if enable circle and diagonal lines looks better but costs performance
    // }


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
        if(!board[y][x])
            is_first_click = false;
        else {
            if(!index_pair_map.empty()) // !!! don't sure maybe wrong place!!!
                index_pair_map.clear();
            board[y][x]->where_to_move(index_pair_map,board, y,x,is_white_turn_to_move); // rewrite this. need to take argument baard onlt by reference. and in all overloaded functions need to check
                                                                                        // create copy poiner then swap squares and call function is king under attack or no if is not then add move if yes do not add
            if((board[y][x]->what_figure() != Figures::black_king) && (board[y][x]->what_figure() != Figures::white_king)) {
                std::pair<int, int> king_index = (is_white_turn_to_move) ? white_king_index : black_king_index;
                for(const auto& it : index_pair_map) {
                    if(it.second){
                        std::unique_ptr<Basic_figure> tmp = std::move(board[it.first.first][it.first.second]);
                        std::swap(board[y][x], board[it.first.first][it.first.second]);
                        bool is_in_check = static_cast<King*>(board[king_index.first][king_index.second].get())->is_king_under_attack(board, king_index.first, king_index.second);
                        std::swap(board[y][x], board[it.first.first][it.first.second]);
                        board[it.first.first][it.first.second] = std::move(tmp);
                        if(is_in_check)
                            index_pair_map.erase(it.first);

                    }
                    else {
                        std::swap(board[y][x], board[it.first.first][it.first.second]);
                        bool is_in_check = static_cast<King*>(board[king_index.first][king_index.second].get())->is_king_under_attack(board, king_index.first, king_index.second);
                        std::swap(board[y][x], board[it.first.first][it.first.second]);
                        if(is_in_check)
                            index_pair_map.erase(it.first);

                    }
                }
            }
        }
    }
    else{
        int new_x_index = (pos.x() - start_x_pos) / 60;
        int new_y_index = (pos.y() - start_y_pos) / 60;


        // if (x == new_x_index && y == new_y_index) { // if you click to same figure twice
        //     is_first_click = false;
        // }

        //board[y][x]->x = pos.x()/60 * 60 - 20;
        //board[y][x]->y = pos.y()/60 * 60 + 10;
        // if(board[new_y_index][new_x_index]) { // if you move your piece to another piece
        //     is_first_click = false;
        // }
        auto it = index_pair_map.find(std::pair<int,int>(new_y_index,new_x_index));
        //if(board[new_y_index][new_x_index]){
            // if(board[y][x]->is_white != board[new_y_index][new_x_index]->is_white ) {
            //     board[new_y_index][new_x_index].reset();
            // }
        if(it != index_pair_map.end()) {
            if(it->second)
                board[new_y_index][new_x_index].reset();
        }

        if(!board[new_y_index][new_x_index] && index_pair_map.contains(std::pair<int,int>(new_y_index,new_x_index))) {
            board[y][x]->x = coordinates_board[new_y_index][new_x_index].x();
            board[y][x]->y = coordinates_board[new_y_index][new_x_index].y();
            board[y][x]->handle_move();
            is_white_turn_to_move = !is_white_turn_to_move;
            if((board[y][x]->what_figure() == Figures::white_king)) {
                white_king_index.first = new_y_index;
                white_king_index.second = new_x_index;

            }
            else if((board[y][x]->what_figure() == Figures::black_king)){
                black_king_index.first = new_y_index;
                black_king_index.second = new_x_index;
            }
            std::swap(board[y][x],board[new_y_index][new_x_index]);
        }


        is_first_click = false;
    }
    update();
}


MainWindow::~MainWindow()
{
    delete ui;
}
