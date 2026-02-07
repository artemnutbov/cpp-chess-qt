#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QPainter>
#include <QDebug>
#include <QPainterPath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow), x(0), y(0), start_x_pos(100), start_y_pos(70), cell_size(60),
    white_button_rect(300,300,cell_size*3,cell_size),black_button_rect(300,300+cell_size+3,cell_size*3,cell_size)
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
    promote_figures[0] = Figures_Name::bishop;
    promote_figures[1] = Figures_Name::rook;
    promote_figures[2] = Figures_Name::knight;
    promote_figures[3] = Figures_Name::queen;
    //board.set_up();

}

void MainWindow::draw_result(QPainter& p) {
    int circle_result_radius = 18;
    QPoint white_king_coordinates(coordinates_board[board.get_king_index(true).first][board.get_king_index(true).second]);
    QPoint black_king_coordinates(coordinates_board[board.get_king_index(false).first][board.get_king_index(false).second]);
    switch (board.what_game_state()) {
    case Game_Result_Status::Stalemate: {
        QColor my_transparent_gray(85,85,85, 230);
        p.setBrush(my_transparent_gray);
        p.drawEllipse(white_king_coordinates, circle_result_radius, circle_result_radius);
        p.drawEllipse(black_king_coordinates, circle_result_radius, circle_result_radius);
        break;
    }
    case Game_Result_Status::White_win:{
        QColor my_transparent_salad_green (131,184,79, 230);
        QColor my_transparent_light_red(224,40,40, 230);

        p.setBrush(my_transparent_salad_green);
        p.drawEllipse(white_king_coordinates, circle_result_radius, circle_result_radius);
        p.setBrush(my_transparent_light_red);
        p.drawEllipse(black_king_coordinates, circle_result_radius, circle_result_radius);

        break;
    }

    case Game_Result_Status::Black_win:{
        QColor my_transparent_salad_green (131,184,79, 230);
        QColor my_transparent_light_red(224,40,40, 230);

        p.setBrush(my_transparent_light_red);
        p.drawEllipse(white_king_coordinates, circle_result_radius, circle_result_radius);

        p.setBrush(my_transparent_salad_green);
        p.drawEllipse(black_king_coordinates, circle_result_radius, circle_result_radius);

        break;
    }
    default:
        break;
    }
}

void MainWindow::draw_legal_moves(QPainter& p) {
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
        if(it.second == Move_types::capture || it.second == Move_types::promote_and_capture){ // idk problem with drawing that circle same transparent color
            path.addEllipse(center_of_cell,half_cell_size,half_cell_size);
            path.addEllipse(center_of_cell,half_cell_size-cicle_thickness,half_cell_size-cicle_thickness);
            p.setBrush(my_transparent_black);
            p.fillPath(path,p.brush());
        }
        else
            p.drawEllipse(center_of_cell , circle_radius, circle_radius);
    }
}

void MainWindow::draw_figure_promotion(QPainter& p) {
    std::pair<int,int> promote_index = board.get_promote_index();
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::white);
    bool is_white_pov = board.get_white_pov();
    if(7 == promote_index.first) {
        QRect promote_rect(coordinates_board[promote_index.first-3][promote_index.second],QSize(cell_size, 4*cell_size));
        p.drawRect(promote_rect);

        p.drawPixmap(coordinates_board[promote_index.first-3][promote_index.second],images_map[Board::name_to_figure(promote_figures[0],!is_white_pov)]);
        p.drawPixmap(coordinates_board[promote_index.first-2][promote_index.second],images_map[Board::name_to_figure(promote_figures[1],!is_white_pov)]);
        p.drawPixmap(coordinates_board[promote_index.first-1][promote_index.second],images_map[Board::name_to_figure(promote_figures[2],!is_white_pov)]);
        p.drawPixmap(coordinates_board[promote_index.first][promote_index.second],images_map[Board::name_to_figure(promote_figures[3],!is_white_pov)]);
    }
    else {
        QRect promote_rect(coordinates_board[promote_index.first][promote_index.second],QSize(cell_size, 4*cell_size));
        p.drawRect(promote_rect);

        p.drawPixmap(coordinates_board[promote_index.first+3][promote_index.second],images_map[Board::name_to_figure(promote_figures[0],is_white_pov)]);
        p.drawPixmap(coordinates_board[promote_index.first+2][promote_index.second],images_map[Board::name_to_figure(promote_figures[1],is_white_pov)]);
        p.drawPixmap(coordinates_board[promote_index.first+1][promote_index.second],images_map[Board::name_to_figure(promote_figures[2],is_white_pov)]);
        p.drawPixmap(coordinates_board[promote_index.first][promote_index.second],images_map[Board::name_to_figure(promote_figures[3],is_white_pov)]);
    }

}
void MainWindow::draw_board_squares(QPainter& p) {
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
}


void MainWindow::draw_choosing_side_buttons(QPainter& p) {
    QColor my_white(235,246,208);
    QFont font("Arial",30);

    font.setBold(true);
    p.setFont(font);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setPen(Qt::black);
    p.setBrush(my_white);
    p.drawRect(white_button_rect);

    p.drawText(white_button_rect, Qt::AlignCenter,"White");

    p.setPen(my_white);
    p.setBrush(Qt::black);
    p.drawRect(black_button_rect);

    p.drawText(black_button_rect,Qt::AlignCenter,"Black");
}

void MainWindow::draw_figures(QPainter& p) {
    for(size_t i = 0;i < 8;++i) {
        for(size_t j = 0;j < 8;++j){
            if(board.valid_index(i,j))
                p.drawPixmap(coordinates_board[i][j].x(), coordinates_board[i][j].y(),images_map[board.what_figure_index(i,j)]);
        }
    }
}
void MainWindow::paintEvent(QPaintEvent *)
{
    // draw board
    QPainter p(this);
    if(click_state == Click_Game_State::choose_play_side)
        draw_choosing_side_buttons(p);
    else {
        draw_board_squares(p);

        if(board.what_game_state() != Game_Result_Status::Playing_Now)
            draw_result(p);

        if(click_state == Click_Game_State::choosing_figure) { // demo of on what figure you click
            draw_legal_moves(p);
        }

        draw_figures(p);

        if(click_state == Click_Game_State::choose_figure_to_promote) { // half done promoting pawn
            // need to separate for black one order for white another
            draw_figure_promotion(p);
        }
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    // check for click
//    if(board.what_game_state() != Game_Result_Status::Playing_Now)
  //      return;
    QPoint pos = event->pos();
    QRect area(start_x_pos,start_y_pos,cell_size*8,cell_size*8);
    // if(!area.contains(pos)) {
    //     return;
    // }

    switch (click_state) {
    case Click_Game_State::choose_play_side: {
        if(white_button_rect.contains(pos)) {
            board.set_up(true);
            click_state = Click_Game_State::none;
        }
        else if(black_button_rect.contains(pos)) {
            board.set_up(false);
            click_state = Click_Game_State::none;
        }
        break;
    }
    case Click_Game_State::none: {
        if(!area.contains(pos)) {
            return;
        }
        x = (pos.x() - start_x_pos) / 60;
        y = (pos.y() - start_y_pos) / 60;
        if(board.valid_index(y,x)) {
            board.all_figure_move(x,y);
            click_state = Click_Game_State::choosing_figure;
        }
        break;
    }
    case Click_Game_State::choosing_figure: {
        if(!area.contains(pos)) {
            return;
        }
        int new_x_index = (pos.x() - start_x_pos) / 60;
        int new_y_index = (pos.y() - start_y_pos) / 60;

        if(!board.action_move(x, y, new_x_index, new_y_index)) {
            click_state = Click_Game_State::none;
            break;
        }
        if(board.is_pawn_promote(new_x_index,new_y_index)) {
            click_state = Click_Game_State::choose_figure_to_promote;
        }
        else{
            click_state = Click_Game_State::none;
            board.set_result_state();
        }

        break;
    }
    case Click_Game_State::choose_figure_to_promote: {
        std::pair<int,int> promote_index = board.get_promote_index();
        if(promote_index.first == 7)
            promote_index.first-=3;
        QRect area_of_promote_figures(coordinates_board[promote_index.first][promote_index.second],QSize(cell_size, cell_size*4));
        if(!area_of_promote_figures.contains(pos)) {
            break;
        }
        int promotion_rank = (pos.y() - start_y_pos) / 60;
        Figures_Name choosen_figure_to_promote;
        if(promote_index.first == 0) {
            board.promotion(*std::prev(promote_figures.end(),1+promotion_rank), board.get_white_pov());
        }
        else {
            board.promotion(promote_figures[promotion_rank-promote_index.first], !board.get_white_pov());
        }
        click_state = Click_Game_State::none;
        board.set_result_state();
        break;
    }
    default:
        break;
    }
    update();
}


MainWindow::~MainWindow()
{
    delete ui;
}
