#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include "Basic_figure.h"
#include <array>
#include "board.h"

using ImagesMap = boost::unordered_flat_map<Figures,QPixmap>;
enum class Click_Game_State {
    choosing_figure, none, move_figure, choose_figure_to_promote, choose_play_side
};


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    int x = 0;
    int y = 0;
    int start_x_pos = 0;
    int start_y_pos = 0;
    int cell_size = 0;
    std::array<std::array<QPoint,8>,8> coordinates_board;
    std::array<Figures_Name,4> promote_figures;
    Click_Game_State click_state = Click_Game_State::choose_play_side;
    QRect white_button_rect;
    QRect black_button_rect;
    Board board;
    ImagesMap images_map;
    void set_up();
    void set_up_images();
    QPixmap create_image(const char* path,bool is_white);
    void draw_result(QPainter& );
    void draw_legal_moves(QPainter& );
    void draw_figure_promotion(QPainter& );
    void draw_board_squares(QPainter& );
    void draw_figures(QPainter& );
    void draw_choosing_side_buttons(QPainter& );
protected:
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent*) override;
};
#endif // MAINWINDOW_H
