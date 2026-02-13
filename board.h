#ifndef BOARD_H
#define BOARD_H
#include <array>

#include "config.h"
// #include <cstdint>

enum class GameResultStatus { kWhiteWin, kBlackWin, kStalemate, kPlayingNow, kNotStarted };

enum class FiguresName {
    kRook,
    kKing,
    kKnight,
    kPawn,
    kBishop,
    kQueen,
};

class Board {
    int black_king_index_;
    int white_king_index_;
    int count_50rule_draw_ = 0;
    bool is_white_turn_to_move_ = true;
    bool is_white_pov_;
    std::array<bool, 64> is_in_start_pos_board_;
    std::array<Figures, 64> board_;
    IndexPair promote_pawn_index_;
    MoveMap index_pair_map_;
    GameResultStatus game_result_status_;

    // zobrist hashing variables
    uint64_t current_hash_ = 0;

    // random numbers for: [PieceType][Square]
    // 0-5 white (P,N,B,R,Q,K), 6-11 black
    static uint64_t zobrist_pieces_[12][64];
    static uint64_t zobrist_side_to_move_;
    static uint64_t zobrist_castling_[4];    // WK, WQ, BK, BQ
    static uint64_t zobrist_en_passant_[8];  // 8 files

    struct MoveInfo {
        MoveTypes move_type;
        Figures our_figure;
        Figures additional_figure = Figures::kNone;
        bool is_promote = false;
        int from_square;
        int to_square;
        uint64_t hash_snapshot;
    };
    std::vector<MoveInfo> history_;

public:
    static Figures NameToFigure(FiguresName, bool);
    void SquareMove(int);

    void UndoMove();
    void SetUp(bool);
    void AllFigureMove(int);
    void Promotion(FiguresName, bool);
    bool ActionMove(int, int);
    void SetResultState();
    bool ValidIndex(int);
    bool IsPawnPromote(int);
    Figures GetFigure(int);
    MoveMap& GetAllLegalMoves();

    static void InitZobrist();
    uint64_t ComputeHash();     // calculates the hash of the CURRENT board
    bool IsRepetition() const;  // checks the history for draws

    GameResultStatus GetGameState() const;
    int GetKingIndex(bool) const;
    int GetPromoteIndex() const;
    bool GetWhitePov() const;
    Board();
};

#endif  // BOARD_H
