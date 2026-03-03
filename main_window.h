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
    static const int kStartBoardXPos = 100;
    static const int kStartBoardYPos = 70;
    static const int kCellSize = 60;
    // for displaying captured pieces
    static const int kStartCapturedFigureDisplayUpX = kStartBoardXPos + 5;
    static const int kStartCapturedFigureDisplayUpY = kStartBoardYPos - kCellSize / 2;
    static const int kStartCapturedFigureDisplayDownX = kStartBoardXPos + 5;
    static const int kStartCapturedFigureDisplayDownY = kStartBoardYPos + kCellSize * 8 + 3;
    static const int kCapturedFigureDisplayStep = kCellSize / 3 + 1;
    static const int kCapturedFigureDisplaySize = kCellSize / 2;

    int current_x_index_ = 0;
    int current_y_index_ = 0;
    bool computer_move_;
    bool is_play_against_bot_;
    bool is_white_pov_;

    std::array<std::array<QPoint, 8>, 8> coordinates_board_;
    std::array<FiguresName, 4> promote_figures_;
    ClickGameState click_state_ = ClickGameState::kChoosePlaySide;

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
    void DrawCapturedPiecesDisplay(QPainter&);

    bool IsGameSetUp();
private slots:
    void RunBenchmark();
    void OnComputerTurn();
    void RestartGame();

    void WhitePushButtonClicked();
    void BlackPushButtonClicked();
    void UndoMovePushButtonClicked();
    void RestartPushButtonClicked();

protected:
    void paintEvent(QPaintEvent*) override;
    void mousePressEvent(QMouseEvent*) override;
};
#endif  // MAIN_WINDOW_H
