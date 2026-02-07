#ifndef BOARD_H
#define BOARD_H
#include "Basic_figure.h"
#include <array>
#include <memory>

enum class Game_Result_Status {
    White_win, Black_win, Stalemate, Playing_Now , Not_Started
};

enum class Figures_Name {
    rook, king, knight, pawn, bishop, queen,
};

class Board {
    std::pair<int,int> black_king_index;
    std::pair<int,int> white_king_index;
    bool is_white_turn_to_move = true;
    bool is_white_pov;
    std::array<std::array<std::unique_ptr<Basic_figure>,8>,8> board;
    IndexPair  promote_pawn_index;
    MoveMap index_pair_map;
    Game_Result_Status game_result_status;
public:

    static Figures name_to_figure(Figures_Name, bool);
    void set_up(bool);
    void all_figure_move(int, int);
    void promotion(Figures_Name, bool);
    bool action_move(int, int, int, int);
    void set_result_state();
    bool valid_index(int, int);
    bool is_pawn_promote(int, int);
    Figures what_figure_index(int, int);
    MoveMap& all_legal_moves();
    Game_Result_Status what_game_state()const;
    const std::pair<int,int>& get_king_index(bool)const;
    const std::pair<int,int>& get_promote_index() const;
    bool get_white_pov()const;
    Board();
};

#endif // BOARD_H
