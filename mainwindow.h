#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMouseEvent>
#include "Basic_figure.h"
#include <array>
#include <memory>

using ImagesMap = boost::unordered_flat_map<Figures,QPixmap>;

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
    std::pair<int,int> black_king_index;
    std::pair<int,int> white_king_index;

    std::array<std::array<QPoint,8>,8> coordinates_board;
    bool is_first_click = false;
    bool is_white_turn_to_move = true;
    std::array<std::array<std::unique_ptr<Basic_figure>,8>,8> board;
    //std::unordered_map<Figures,QPixmap> images_map;
    ImagesMap images_map;
    MoveMap index_pair_map;
    void set_up();
    void set_up_images();
    QPixmap create_image(const char* path,bool is_white);
protected:
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent*) override;
};
#endif // MAINWINDOW_H
