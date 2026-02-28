#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QMouseEvent>
#include <memory>

#include "board.h"

using ImagesMap = boost::unordered_flat_map<Figures, QPixmap>;
enum class ClickGameState {
    kChoosingFigure,
    kNone,
    kMoveFigure,
    kChooseFigureToPromote,
    kChoosePlaySide
};

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow* ui;
    static const int start_x_pos_ = 100;
    static const int start_y_pos_ = 70;
    static const int cell_size_ = 60;

    int current_x_index_ = 0;
    int current_y_index_ = 0;
    bool computer_move_;

    std::array<std::array<QPoint, 8>, 8> coordinates_board_;
    std::array<FiguresName, 4> promote_figures_;
    ClickGameState click_state_ = ClickGameState::kChoosePlaySide;
    QRect white_button_rect_;
    QRect black_button_rect_;
    QRect gray_button_rect_;

    std::unique_ptr<Board> board_;
    ImagesMap images_map_;
    void SetUp();
    void SetUpImages();
    QPixmap CreateImage(const char* path, bool is_white);
    void DrawResult(QPainter&);
    void DrawLegalMoves(QPainter&);
    void DrawFigurePromotion(QPainter&);
    void DrawBoardSquares(QPainter&);
    void DrawFigures(QPainter&);
    void DrawChoosingSideButtons(QPainter&);
    void DrawUndoMoveButton(QPainter&);

private slots:
    void RunBenchmark();
    void OnComputerTurn();
    void RestartGame();

protected:
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
};
#endif  // MAIN_WINDOW_H
