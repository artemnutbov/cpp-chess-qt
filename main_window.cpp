#include "main_window.h"

#include <QDebug>
#include <QPainter>
#include <QPainterPath>
#include <QThreadPool>
#include <QTimer>
// #include <chrono>

#include "./ui_main_window.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), current_x_index_(0), current_y_index_(0) {
    ui->setupUi(this);
    connect(ui->WhitePlayerPushButton, &QPushButton::clicked, this,
            &MainWindow::WhitePushButtonClicked);
    connect(ui->BlackPlayerPushButton, &QPushButton::clicked, this,
            &MainWindow::BlackPushButtonClicked);
    connect(ui->UndoMovePushButton, &QPushButton::clicked, this,
            &MainWindow::UndoMovePushButtonClicked);
    connect(ui->RestartPushButton, &QPushButton::clicked, this,
            &MainWindow::RestartPushButtonClicked);
    ui->UndoMovePushButton->hide();
    ui->RestartPushButton->hide();
    this->setStyleSheet("QMainWindow { background-color: hsla(0,0%,50%,0.6); }");
    SetUpImages();
    SetUp();
}

QPixmap MainWindow::CreateImage(const char* path, bool is_white) {
    QPixmap img(path);  // or a normal file path

    QBitmap mask;
    if (is_white)
        mask = img.createMaskFromColor(Qt::black);
    else
        mask = img.createMaskFromColor(Qt::white);
    img.setMask(mask);
    img = img.scaled(kCellSize, kCellSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    return img;
}
void MainWindow::SetUpImages() {
    images_map_[Figures::kWhitePawn] = CreateImage(":/img/images/white_pawn.png", true);
    images_map_[Figures::kWhiteBishop] = CreateImage(":/img/images/white_bishop.png", true);
    images_map_[Figures::kWhiteKnight] = CreateImage(":/img/images/white_knight.png", true);
    images_map_[Figures::kWhiteRook] = CreateImage(":/img/images/white_rook.png", true);
    images_map_[Figures::kWhiteQueen] = CreateImage(":/img/images/white_queen.png", true);
    images_map_[Figures::kWhiteKing] = CreateImage(":/img/images/white_king.png", true);

    images_map_[Figures::kBlackPawn] = CreateImage(":/img/images/black_pawn.png", false);
    images_map_[Figures::kBlackBishop] = CreateImage(":/img/images/black_bishop.png", false);
    images_map_[Figures::kBlackKnight] = CreateImage(":/img/images/black_knight.png", false);
    images_map_[Figures::kBlackRook] = CreateImage(":/img/images/black_rook.png", false);
    images_map_[Figures::kBlackQueen] = CreateImage(":/img/images/black_queen.png", false);
    images_map_[Figures::kBlackKing] = CreateImage(":/img/images/black_king.png", false);
}
void MainWindow::SetUp() {
    // coordinates_board_ SetUp
    for (size_t i = 0; i < 8; ++i) {
        for (size_t j = 0; j < 8; ++j) {
            coordinates_board_[i][j] =
                QPoint(kStartBoardXPos + kCellSize * j, kStartBoardYPos + kCellSize * i);
        }
    }
    promote_figures_[0] = FiguresName::kBishop;
    promote_figures_[1] = FiguresName::kRook;
    promote_figures_[2] = FiguresName::kKnight;
    promote_figures_[3] = FiguresName::kQueen;
}

void MainWindow::DrawResult(QPainter& p) {
    int circle_result_radius = 18;
    QPoint white_king_coordinates(
        coordinates_board_[board_->GetKingIndex(true) >> 3][board_->GetKingIndex(true) & 7]);
    QPoint black_king_coordinates(
        coordinates_board_[board_->GetKingIndex(false) >> 3][board_->GetKingIndex(false) & 7]);

    switch (board_->GetGameState()) {
        case GameResultStatus::kStalemate: {
            QColor my_transparent_gray(85, 85, 85, 230);
            p.setBrush(my_transparent_gray);
            p.drawEllipse(white_king_coordinates, circle_result_radius, circle_result_radius);
            p.drawEllipse(black_king_coordinates, circle_result_radius, circle_result_radius);
            break;
        }
        case GameResultStatus::kWhiteWin: {
            QColor my_transparent_salad_green(131, 184, 79, 230);
            QColor my_transparent_light_red(224, 40, 40, 230);

            p.setBrush(my_transparent_salad_green);
            p.drawEllipse(white_king_coordinates, circle_result_radius, circle_result_radius);
            p.setBrush(my_transparent_light_red);
            p.drawEllipse(black_king_coordinates, circle_result_radius, circle_result_radius);

            break;
        }

        case GameResultStatus::kBlackWin: {
            QColor my_transparent_salad_green(131, 184, 79, 230);
            QColor my_transparent_light_red(224, 40, 40, 230);

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

void MainWindow::DrawLegalMoves(QPainter& p) {
    QColor my_transparent_yellow(245, 246, 130, 160);
    p.setBrush(my_transparent_yellow);
    p.drawRect(coordinates_board_[current_y_index_][current_x_index_].x(),
               coordinates_board_[current_y_index_][current_x_index_].y(), kCellSize, kCellSize);

    QColor my_transparent_black(0, 0, 0, 20);

    int circle_radius = kCellSize / 6;
    int half_kCellSize = kCellSize / 2;
    int cicle_thickness = 6;
    QPainterPath path;
    p.setBrush(my_transparent_black);
    for (const auto& it : board_->GetAllLegalMoves()) {
        QPointF center_of_cell((it.first & 7) * kCellSize + kStartBoardXPos + half_kCellSize,
                               (it.first >> 3) * kCellSize + half_kCellSize + kStartBoardYPos);

        if (it.second == MoveTypes::kCapture ||
            it.second == MoveTypes::kPromoteCapture) {  // idk problem with drawing that circle
                                                            // same transparent color
            path.addEllipse(center_of_cell, half_kCellSize, half_kCellSize);
            path.addEllipse(center_of_cell, half_kCellSize - cicle_thickness,
                            half_kCellSize - cicle_thickness);
            p.setBrush(my_transparent_black);
            p.fillPath(path, p.brush());
        } else
            p.drawEllipse(center_of_cell, circle_radius, circle_radius);
    }
}

void MainWindow::DrawFigurePromotion(QPainter& p) {
    int promote_index = board_->GetPromoteIndex();
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::white);
    bool is_white_pov = board_->GetWhitePov();
    if (7 == promote_index >> 3) {
        QRect promote_rect(coordinates_board_[(promote_index >> 3) - 3][promote_index & 7],
                           QSize(kCellSize, 4 * kCellSize));
        p.drawRect(promote_rect);

        p.drawPixmap(coordinates_board_[(promote_index >> 3) - 3][promote_index & 7],
                     images_map_[Board::NameToFigure(promote_figures_[0], !is_white_pov)]);
        p.drawPixmap(coordinates_board_[(promote_index >> 3) - 2][promote_index & 7],
                     images_map_[Board::NameToFigure(promote_figures_[1], !is_white_pov)]);
        p.drawPixmap(coordinates_board_[(promote_index >> 3) - 1][promote_index & 7],
                     images_map_[Board::NameToFigure(promote_figures_[2], !is_white_pov)]);
        p.drawPixmap(coordinates_board_[(promote_index >> 3)][promote_index & 7],
                     images_map_[Board::NameToFigure(promote_figures_[3], !is_white_pov)]);
    } else {
        QRect promote_rect(coordinates_board_[(promote_index >> 3)][promote_index & 7],
                           QSize(kCellSize, 4 * kCellSize));
        p.drawRect(promote_rect);

        p.drawPixmap(coordinates_board_[(promote_index >> 3) + 3][promote_index & 7],
                     images_map_[Board::NameToFigure(promote_figures_[0], is_white_pov)]);
        p.drawPixmap(coordinates_board_[(promote_index >> 3) + 2][promote_index & 7],
                     images_map_[Board::NameToFigure(promote_figures_[1], is_white_pov)]);
        p.drawPixmap(coordinates_board_[(promote_index >> 3) + 1][promote_index & 7],
                     images_map_[Board::NameToFigure(promote_figures_[2], is_white_pov)]);
        p.drawPixmap(coordinates_board_[(promote_index >> 3)][promote_index & 7],
                     images_map_[Board::NameToFigure(promote_figures_[3], is_white_pov)]);
    }
}
void MainWindow::DrawBoardSquares(QPainter& p) {
    QColor my_white(235, 246, 208);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setPen(Qt::NoPen);
    p.setBrush(my_white);

    for (size_t i = 0; i < 4; ++i) {
        for (size_t j = 0; j < 8; ++j) {
            if (j % 2 == 0)
                p.drawRect(coordinates_board_[i * 2][j].x(), coordinates_board_[i * 2][j].y(),
                           kCellSize, kCellSize);
            else
                p.drawRect(coordinates_board_[1 + i * 2][j].x(),
                           coordinates_board_[1 + i * 2][j].y(), kCellSize, kCellSize);
        }
    }
    QColor my_green(115, 149, 82);
    p.setPen(Qt::NoPen);
    p.setBrush(my_green);

    for (size_t i = 0; i < 4; ++i) {
        for (size_t j = 0; j < 8; ++j) {
            if (j % 2 == 0)
                p.drawRect(coordinates_board_[1 + i * 2][j].x(),
                           coordinates_board_[1 + i * 2][j].y(), kCellSize, kCellSize);
            else
                p.drawRect(coordinates_board_[i * 2][j].x(), coordinates_board_[i * 2][j].y(),
                           kCellSize, kCellSize);
        }
    }
}

void MainWindow::DrawFigures(QPainter& p) {
    for (size_t i = 0; i < 8; ++i) {
        for (size_t j = 0; j < 8; ++j) {
            if (board_->ValidIndex(i * 8 + j))
                p.drawPixmap(coordinates_board_[i][j].x(), coordinates_board_[i][j].y(),
                             images_map_[board_->GetFigure(i * 8 + j)]);
        }
    }
}

void MainWindow::DrawCapturedPiecesDisplay(QPainter& p) {
    int current_display_up_x = kStartCapturedFigureDisplayUpX;
    int current_display_down_x = kStartCapturedFigureDisplayDownX;

    for (auto captured_figure : board_->GetCaptureFigures()) {
        QPixmap img = images_map_[captured_figure];
        img = img.scaled(kCapturedFigureDisplaySize, kCapturedFigureDisplaySize,
                         Qt::KeepAspectRatio, Qt::SmoothTransformation);
        bool is_white = Config::FigureToSide(captured_figure);
        if (is_white_pov_) {
            if (!is_white) {
                p.drawPixmap(current_display_down_x, kStartCapturedFigureDisplayDownY, img);
                current_display_down_x += kCapturedFigureDisplayStep;

            } else {
                p.drawPixmap(current_display_up_x, kStartCapturedFigureDisplayUpY, img);
                current_display_up_x += kCapturedFigureDisplayStep;
            }

        } else {
            if (is_white) {
                p.drawPixmap(current_display_down_x, kStartCapturedFigureDisplayDownY, img);
                current_display_down_x += kCapturedFigureDisplayStep;

            } else {
                p.drawPixmap(current_display_up_x, kStartCapturedFigureDisplayUpY, img);
                current_display_up_x += kCapturedFigureDisplayStep;
            }
        }
    }
}
void MainWindow::paintEvent(QPaintEvent*) {
    // draw board
    QPainter p(this);
    if (click_state_ != ClickGameState::kChoosePlaySide) {
        DrawBoardSquares(p);

        if (board_->GetGameState() != GameResultStatus::kPlayingNow &&
            board_->GetGameState() != GameResultStatus::kNotStarted) {
            DrawResult(p);
            QTimer::singleShot(1000, this, &MainWindow::RestartGame);
        }
        if (click_state_ == ClickGameState::kChoosingFigure) {  // demo of on what figure you click
            DrawLegalMoves(p);
        }

        DrawFigures(p);

        DrawCapturedPiecesDisplay(p);

        if (click_state_ == ClickGameState::kChooseFigureToPromote) {
            DrawFigurePromotion(p);
        }
    }
}

void MainWindow::RestartGame() {
    click_state_ = ClickGameState::kChoosePlaySide;
    ui->RestartPushButton->show();
}

void MainWindow::RunBenchmark() {  // test function
    computer_move_ = true;
    // qDebug() << "Starting Benchmark at Depth " << depth << "...";

    // auto start_time = std::chrono::high_resolution_clock::now();

    board_->MakeBotMove(board_->SearchRoot(1000));  // call computer move
    // auto end_time = std::chrono::high_resolution_clock::now();

    // auto duration =
    //     std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();

    // qDebug() << "Time taken: " << duration << " ms " << nodes;

    // qDebug() << "Time taken: " << duration << " ms ";
    board_->SetResultState();
    update();
    computer_move_ = false;
}

void MainWindow::OnComputerTurn() {
    QThreadPool::globalInstance()->start([this]() { RunBenchmark(); });
}

void MainWindow::mousePressEvent(QMouseEvent* event) {
    // check for click
    if (click_state_ == ClickGameState::kChoosePlaySide ||
        board_->GetGameState() != GameResultStatus::kPlayingNow &&
            board_->GetGameState() != GameResultStatus::kNotStarted)
        return;
    QPoint pos = event->pos();
    QRect area(kStartBoardXPos, kStartBoardYPos, kCellSize * 8, kCellSize * 8);
    switch (click_state_) {
        case ClickGameState::kNone: {
            if (computer_move_) return;
            if (area.contains(pos)) {
                current_x_index_ = (pos.x() - kStartBoardXPos) / kCellSize;
                current_y_index_ = (pos.y() - kStartBoardYPos) / kCellSize;
                if (board_->ValidIndex(current_y_index_ * 8 + current_x_index_)) {
                    board_->AllFigureMove(current_y_index_ * 8 + current_x_index_);
                    click_state_ = ClickGameState::kChoosingFigure;
                }
            }
            break;
        }
        case ClickGameState::kChoosingFigure: {
            if (computer_move_) return;
            if (!area.contains(pos)) {
                return;
            }
            int new_x_index = (pos.x() - kStartBoardXPos) / kCellSize;
            int new_y_index = (pos.y() - kStartBoardYPos) / kCellSize;
            if (!board_->ActionMove(current_x_index_ + current_y_index_ * 8,
                                    new_x_index + new_y_index * 8)) {
                click_state_ = ClickGameState::kNone;
                break;
            }
            if (board_->IsPawnPromote(new_x_index + new_y_index * 8)) {
                click_state_ = ClickGameState::kChooseFigureToPromote;
            } else {
                click_state_ = ClickGameState::kNone;
                board_->SetResultState();
                if (board_->GetGameState() == GameResultStatus::kPlayingNow && is_play_against_bot_)
                    QTimer::singleShot(100, this, &MainWindow::OnComputerTurn);
            }
            break;
        }

        case ClickGameState::kChooseFigureToPromote: {
            if (computer_move_) return;
            int promote_index = board_->GetPromoteIndex();
            if ((promote_index >> 3) == 7) promote_index -= 3 * 8;
            QRect area_of_promote_figures_(
                coordinates_board_[promote_index >> 3][promote_index & 7],
                QSize(kCellSize, kCellSize * 4));
            if (!area_of_promote_figures_.contains(pos)) {
                break;
            }
            int Promotion_rank = (pos.y() - kStartBoardYPos) / kCellSize;
            FiguresName choosen_figure_to_promote;
            if ((promote_index >> 3) == 0) {
                board_->Promotion(*std::prev(promote_figures_.end(), 1 + Promotion_rank),
                                  board_->GetWhitePov());
            } else {
                board_->Promotion(promote_figures_[Promotion_rank - (promote_index >> 3)],
                                  !board_->GetWhitePov());
            }
            click_state_ = ClickGameState::kNone;
            board_->SetResultState();

            if (is_play_against_bot_) QTimer::singleShot(100, this, &MainWindow::OnComputerTurn);
            break;
        }
        default:
            break;
    }
    update();
}

bool MainWindow::IsGameSetUp() {
    if (ui->BotPlayButton->isChecked())
        computer_move_ = is_play_against_bot_ = true;
    else if (ui->LocalPlayButton->isChecked())
        is_play_against_bot_ = computer_move_ = false;
    else
        return false;
    ui->WhitePlayerPushButton->hide();
    ui->BlackPlayerPushButton->hide();
    ui->LocalPlayButton->hide();
    ui->BotPlayButton->hide();

    ui->UndoMovePushButton->show();
    return true;
}

void MainWindow::WhitePushButtonClicked() {
    if (!IsGameSetUp()) return;

    is_white_pov_ = true;
    board_ = std::make_unique<Board>(true);
    computer_move_ = false;
    click_state_ = ClickGameState::kNone;
    update();
}

void MainWindow::BlackPushButtonClicked() {
    if (!IsGameSetUp()) return;
    is_white_pov_ = false;
    board_ = std::make_unique<Board>(is_white_pov_);

    if (is_play_against_bot_) QTimer::singleShot(1000, this, &MainWindow::OnComputerTurn);

    click_state_ = ClickGameState::kNone;
    update();
}

void MainWindow::UndoMovePushButtonClicked() {
    if (!computer_move_ && click_state_ != ClickGameState::kChoosePlaySide) {
        board_->UndoMove();
        board_->UndoMove();
        update();
    }
}

void MainWindow::RestartPushButtonClicked() {
    ui->WhitePlayerPushButton->show();
    ui->BlackPlayerPushButton->show();
    ui->LocalPlayButton->show();
    ui->BotPlayButton->show();

    ui->UndoMovePushButton->hide();
    ui->RestartPushButton->hide();

    board_.reset();
    update();
}

MainWindow::~MainWindow() {
    delete ui;
}
