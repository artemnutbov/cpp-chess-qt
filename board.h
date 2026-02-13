#ifndef BOARD_H
#define BOARD_H
#include "BasicFigure.h"
#include <array>
//#include <cstdint>

enum class Game_Result_Status {
    White_win, Black_win, Stalemate, Playing_Now , Not_Started
};

enum class Figures_Name {
    rook, king, knight, pawn, bishop, queen,
};

class Board {
    int black_king_index;
    int white_king_index;
    int count_50rule_draw = 0;
    bool is_white_turn_to_move = true;
    bool is_white_pov;
    std::array<bool, 64> is_in_start_pos_board;
    std::array<Figures ,64> board;
    IndexPair  promote_pawn_index;
    MoveMap index_pair_map;
    Game_Result_Status game_result_status;

    // zobrist hashing variables
    uint64_t current_hash = 0;

    // random numbers for: [PieceType][Square]
    // 0-5 white (P,N,B,R,Q,K), 6-11 black
    static uint64_t zobrist_pieces[12][64];
    static uint64_t zobrist_side_to_move;
    static uint64_t zobrist_castling[4]; // WK, WQ, BK, BQ
    static uint64_t zobrist_en_passant[8]; // 8 files

    struct MoveInfo {
        Move_types move_type;
        Figures our_figure;
        Figures additional_figure = Figures::none;
        bool is_promote = false;
        int from_square;
        int to_square;
        uint64_t hash_snapshot;
    };
    std::vector<MoveInfo> history;
public:
    static std::pair<int, int> to_pair(int square);
    static int to_int(int x, int y);
    static Figures name_to_figure(Figures_Name, bool);
    void square_move(int);

    void undo_move();
    void set_up(bool);
    void all_figure_move(int);
    void promotion(Figures_Name, bool);
    bool action_move(int, int);
    void set_result_state();
    bool valid_index(int);
    bool is_pawn_promote(int);
    Figures what_figure_index(int);
    MoveMap& all_legal_moves();

    static void init_zobrist();
    uint64_t compute_hash();    // calculates the hash of the CURRENT board
    bool is_repetition() const; // checks the history for draws

    Game_Result_Status what_game_state()const;
    int get_king_index(bool)const;
    int get_promote_index() const;
    bool get_white_pov()const;
    Board();
};

#endif // BOARD_H
