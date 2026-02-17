#ifndef BOARD_H
#define BOARD_H
#include <array>

#include "config.h"
enum class GameResultStatus { kWhiteWin, kBlackWin, kStalemate, kPlayingNow, kNotStarted };

enum class FiguresName {
    kRook,
    kKing,
    kKnight,
    kPawn,
    kBishop,
    kQueen,
};

struct Move {
    int from;
    int to;
    MoveTypes type;
};
struct MoveList {
    std::array<Move, 256> moves;  // a little bit more move than max(218)

    int count = 0;

    void push_back(const Move& m) {
        moves[count++] = m;
    }

    Move* begin() {
        return &moves[0];
    }
    Move* end() {
        return &moves[count];
    }

    const Move* begin() const {
        return &moves[0];
    }
    const Move* end() const {
        return &moves[count];
    }

    bool empty() const {
        return count == 0;
    }
    int size() const {
        return count;
    }

    Move& operator[](int index) {
        return moves[index];
    }
    const Move& operator[](int index) const {
        return moves[index];
    }
};
class Board {
    int black_king_index_;
    int white_king_index_;
    int count_50rule_draw_ = 0;
    bool is_white_turn_to_move_ = true;
    bool is_white_pov_;
    std::array<bool, 6> is_in_start_pos_board_;  // is_in_start_pos_board_{WhiteKing,BlackKing
                                                 // LeftBottomRook, RightBottomRook,
                                                 // LeftTopRook, RightTopRook}
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

    struct MoveUndoInfo {
        MoveTypes move_type;
        Figures our_figure;
        Figures additional_figure = Figures::kNone;
        bool is_promote = false;
        int from_square;
        int to_square;
        uint64_t hash_snapshot;
    };

    std::vector<MoveUndoInfo> history_;
    int QuiescenceSearch(int, int);
    int GetFigureValue(Figures figure);
    MoveList GenerateMoves();
    MoveList GenerateCaptures();

public:
    int RankMove(const Move& move);  // rate moves
    int Evaluate();
    int Negamax(int, int, int);
    void MakeBotMove(Move bot_move);

    long long Perft(int depth);

    Move SearchRoot(int depth);
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
