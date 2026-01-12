#ifndef BOARD_H
#define BOARD_H
#include "Basic_figure.h"
#include <array>
#include <memory>

enum class Game_Result_Status {
    White_win, Black_win, Stalemate, Playing_Now , Not_Started
};


class Board {
    std::pair<int,int> black_king_index;
    std::pair<int,int> white_king_index;
    bool is_white_turn_to_move = true;
    std::array<std::array<std::unique_ptr<Basic_figure>,8>,8> board;
    MoveMap index_pair_map;
    Game_Result_Status game_result_status;
public:
    void set_up();
    void all_figure_move(int, int);
    void action_move(int, int, int, int);
    bool valid_index(int, int);
    Figures what_figure_index(int, int);
    MoveMap& all_legal_moves();
    Board();
};

#endif // BOARD_H
