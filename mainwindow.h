#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include "Basic_figure.h"
#include <array>
#include "board.h"

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

    std::array<std::array<QPoint,8>,8> coordinates_board;
    bool is_first_click = false;
    Board board;
    ImagesMap images_map;
    void set_up();
    void set_up_images();
    QPixmap create_image(const char* path,bool is_white);
protected:
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent*) override;
};
#endif // MAINWINDOW_H
