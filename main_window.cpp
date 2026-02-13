#include "main_window.h"

#include <QDebug>
#include <QPainter>
#include <QPainterPath>

#include "./ui_main_window.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      current_x_index(0),
      current_y_index(0),
      start_x_pos_(100),
      start_y_pos_(70),
      cell_size_(60),
      white_button_rect_(300, 300, cell_size_ * 3, cell_size_),
      black_button_rect_(300, 300 + cell_size_ + 3, cell_size_ * 3, cell_size_),
      gray_button_rect_(start_x_pos_ + cell_size_ * 8 + 2, start_y_pos_ + cell_size_ * 7,
                        cell_size_ * 2, cell_size_) {
    ui->setupUi(this);
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
    img = img.scaled(60, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    return img;
}
void MainWindow::SetUpImages() {
    images_map_[Figures::kWhitePawn] =
        CreateImage("C:/Users/38096/Documents/my_chess_game/images/white_pawn.png", true);
    images_map_[Figures::kWhiteBishop] =
        CreateImage("C:/Users/38096/Documents/my_chess_game/images/white_bishop.png", true);
    images_map_[Figures::kWhiteKnight] =
        CreateImage("C:/Users/38096/Documents/my_chess_game/images/white_knight.png", true);
    images_map_[Figures::kWhiteRook] =
        CreateImage("C:/Users/38096/Documents/my_chess_game/images/white_rook.png", true);
    images_map_[Figures::kWhiteQueen] =
        CreateImage("C:/Users/38096/Documents/my_chess_game/images/white_queen.png", true);

    images_map_[Figures::kWhiteKing] =
        CreateImage("C:/Users/38096/Documents/my_chess_game/images/white_king.png", true);

    images_map_[Figures::kBlackPawn] =
        CreateImage("C:/Users/38096/Documents/my_chess_game/images/black_pawn.png", false);
    images_map_[Figures::kBlackBishop] =
        CreateImage("C:/Users/38096/Documents/my_chess_game/images/black_bishop.png", false);
    images_map_[Figures::kBlackKnight] =
        CreateImage("C:/Users/38096/Documents/my_chess_game/images/black_knight.png", false);
    images_map_[Figures::kBlackRook] =
        CreateImage("C:/Users/38096/Documents/my_chess_game/images/black_rook.png", false);
    images_map_[Figures::kBlackQueen] =
        CreateImage("C:/Users/38096/Documents/my_chess_game/images/black_queen.png", false);
    images_map_[Figures::kBlackKing] =
        CreateImage("C:/Users/38096/Documents/my_chess_game/images/black_king.png", false);
}
void MainWindow::SetUp() {
    // coordinates_board_ SetUp
    for (size_t i = 0; i < 8; ++i) {
        for (size_t j = 0; j < 8; ++j) {
            coordinates_board_[i][j] =
                QPoint(start_x_pos_ + cell_size_ * j, start_y_pos_ + cell_size_ * i);
        }
    }
    promote_figures_[0] = FiguresName::kBishop;
    promote_figures_[1] = FiguresName::kRook;
    promote_figures_[2] = FiguresName::kKnight;
    promote_figures_[3] = FiguresName::kQueen;
    Board::InitZobrist();
    // board.SetUp(true);
}

void MainWindow::DrawResult(QPainter& p) {
    int circle_result_radius = 18;
    QPoint white_king_coordinates(
        coordinates_board_[board_.GetKingIndex(true) >> 3][board_.GetKingIndex(true) & 7]);
    QPoint black_king_coordinates(
        coordinates_board_[board_.GetKingIndex(false) >> 3][board_.GetKingIndex(false) & 7]);

    switch (board_.GetGameState()) {
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
    p.drawRect(coordinates_board_[current_y_index][current_x_index].x(),
               coordinates_board_[current_y_index][current_x_index].y(), cell_size_, cell_size_);

    QColor my_transparent_black(0, 0, 0, 20);

    int circle_radius = cell_size_ / 6;
    int half_cell_size_ = cell_size_ / 2;
    int cicle_thickness = 6;
    QPainterPath path;
    p.setBrush(my_transparent_black);
    for (const auto& it : board_.GetAllLegalMoves()) {
        QPointF center_of_cell((it.first & 7) * cell_size_ + start_x_pos_ + half_cell_size_,
                               (it.first >> 3) * cell_size_ + half_cell_size_ + start_y_pos_);

        if (it.second == MoveTypes::kCapture ||
            it.second == MoveTypes::kPromoteCapture) {  // idk problem with drawing that circle
                                                            // same transparent color
            path.addEllipse(center_of_cell, half_cell_size_, half_cell_size_);
            path.addEllipse(center_of_cell, half_cell_size_ - cicle_thickness,
                            half_cell_size_ - cicle_thickness);
            p.setBrush(my_transparent_black);
            p.fillPath(path, p.brush());
        } else
            p.drawEllipse(center_of_cell, circle_radius, circle_radius);
    }
}

void MainWindow::DrawFigurePromotion(QPainter& p) {
    int promote_index = board_.GetPromoteIndex();
    p.setPen(Qt::NoPen);
    p.setBrush(Qt::white);
    bool is_white_pov = board_.GetWhitePov();
    if (7 == promote_index >> 3) {
        QRect promote_rect(coordinates_board_[(promote_index >> 3) - 3][promote_index & 7],
                           QSize(cell_size_, 4 * cell_size_));
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
                           QSize(cell_size_, 4 * cell_size_));
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
                           cell_size_, cell_size_);
            else
                p.drawRect(coordinates_board_[1 + i * 2][j].x(),
                           coordinates_board_[1 + i * 2][j].y(), cell_size_, cell_size_);
        }
    }
    QColor my_green(115, 149, 82);
    p.setPen(Qt::NoPen);
    p.setBrush(my_green);

    for (size_t i = 0; i < 4; ++i) {
        for (size_t j = 0; j < 8; ++j) {
            if (j % 2 == 0)
                p.drawRect(coordinates_board_[1 + i * 2][j].x(),
                           coordinates_board_[1 + i * 2][j].y(), cell_size_, cell_size_);
            else
                p.drawRect(coordinates_board_[i * 2][j].x(), coordinates_board_[i * 2][j].y(),
                           cell_size_, cell_size_);
        }
    }
}

void MainWindow::DrawUndoMoveButton(QPainter& p) {
    p.setPen(Qt::gray);
    p.setBrush(Qt::gray);
    p.drawRect(gray_button_rect_);
}

void MainWindow::DrawChoosingSideButtons(QPainter& p) {
    QColor my_white(235, 246, 208);
    QFont font("Arial", 30);

    font.setBold(true);
    p.setFont(font);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setPen(Qt::black);
    p.setBrush(my_white);
    p.drawRect(white_button_rect_);

    p.drawText(white_button_rect_, Qt::AlignCenter, "White");

    p.setPen(my_white);
    p.setBrush(Qt::black);
    p.drawRect(black_button_rect_);

    p.drawText(black_button_rect_, Qt::AlignCenter, "Black");
}

void MainWindow::DrawFigures(QPainter& p) {
    for (size_t i = 0; i < 8; ++i) {
        for (size_t j = 0; j < 8; ++j) {
            if (board_.ValidIndex(i * 8 + j))
                p.drawPixmap(coordinates_board_[i][j].x(), coordinates_board_[i][j].y(),
                             images_map_[board_.GetFigure(i * 8 + j)]);
        }
    }
}
void MainWindow::paintEvent(QPaintEvent*) {
    // draw board
    QPainter p(this);
    if (click_state_ == ClickGameState::kChoosePlaySide)
        DrawChoosingSideButtons(p);
    else {
        DrawBoardSquares(p);

        if (board_.GetGameState() != GameResultStatus::kPlayingNow) DrawResult(p);

        if (click_state_ == ClickGameState::kChoosingFigure) {  // demo of on what figure you click
            DrawLegalMoves(p);
        }

        DrawUndoMoveButton(p);
        DrawFigures(p);

        if (click_state_ == ClickGameState::kChooseFigureToPromote) {
            DrawFigurePromotion(p);
        }
    }
}

void MainWindow::mousePressEvent(QMouseEvent* event) {
    // check for click
    if (board_.GetGameState() != GameResultStatus::kPlayingNow &&
        board_.GetGameState() != GameResultStatus::kNotStarted)
        return;
    QPoint pos = event->pos();
    QRect area(start_x_pos_, start_y_pos_, cell_size_ * 8, cell_size_ * 8);

    switch (click_state_) {
        case ClickGameState::kChoosePlaySide: {
            if (white_button_rect_.contains(pos)) {
                board_.SetUp(true);
                click_state_ = ClickGameState::kNone;
            } else if (black_button_rect_.contains(pos)) {
                board_.SetUp(false);
                click_state_ = ClickGameState::kNone;
            }
            break;
        }
        case ClickGameState::kNone: {
            if (area.contains(pos)) {
                current_x_index = (pos.x() - start_x_pos_) / 60;
                current_y_index = (pos.y() - start_y_pos_) / 60;
                if (board_.ValidIndex(current_y_index * 8 + current_x_index)) {
                    board_.AllFigureMove(current_y_index * 8 + current_x_index);
                    click_state_ = ClickGameState::kChoosingFigure;
                }
            } else if (gray_button_rect_.contains(pos)) {
                board_.UndoMove();
            }

            break;
        }
        case ClickGameState::kChoosingFigure: {
            if (!area.contains(pos)) {
                return;
            }
            int new_x_index = (pos.x() - start_x_pos_) / 60;
            int new_y_index = (pos.y() - start_y_pos_) / 60;
            if (!board_.ActionMove(current_x_index + current_y_index * 8,
                                   new_x_index + new_y_index * 8)) {
                click_state_ = ClickGameState::kNone;
                break;
            }
            if (board_.IsPawnPromote(new_x_index + new_y_index * 8)) {
                click_state_ = ClickGameState::kChooseFigureToPromote;
            } else {
                click_state_ = ClickGameState::kNone;
                board_.SetResultState();
            }

            break;
        }

        case ClickGameState::kChooseFigureToPromote: {
            int promote_index = board_.GetPromoteIndex();
            if ((promote_index >> 3) == 7) promote_index -= 3 * 8;
            QRect area_of_promote_figures_(
                coordinates_board_[promote_index >> 3][promote_index & 7],
                QSize(cell_size_, cell_size_ * 4));
            if (!area_of_promote_figures_.contains(pos)) {
                break;
            }
            int Promotion_rank = (pos.y() - start_y_pos_) / 60;
            FiguresName choosen_figure_to_promote;
            if ((promote_index >> 3) == 0) {
                board_.Promotion(*std::prev(promote_figures_.end(), 1 + Promotion_rank),
                                 board_.GetWhitePov());
            } else {
                board_.Promotion(promote_figures_[Promotion_rank - (promote_index >> 3)],
                                 !board_.GetWhitePov());
            }
            click_state_ = ClickGameState::kNone;
            board_.SetResultState();
            break;
        }
        default:
            break;
    }
    update();
}

MainWindow::~MainWindow() {
    delete ui;
}
