#ifndef BASIC_FIGURE_H
#define BASIC_FIGURE_H

#include <boost/unordered/unordered_flat_map.hpp>

struct PairHash {
    std::size_t operator()(const std::pair<int,int>& p) const noexcept {
        return (std::hash<int>()(p.first) << 1) ^ std::hash<int>()(p.second);
    }
};

enum class Move_types {
    move_to_empty_square, short_castling, long_castling, capture, promote_to_empty_square,  promote_and_capture
};

using IndexPair = std::pair<std::pair<int,int>,Move_types>;
using MoveMap = boost::unordered_flat_map<std::pair<int,int>, Move_types, PairHash>;

enum class Figures {
    white_pawn,white_king,white_knight,white_queen,white_rook,white_bishop,
    black_pawn,black_king,black_knight,black_queen,black_rook,black_bishop,none
};


class Basic_figure {
protected:
    Figures figure;
    bool is_white;

public:

    bool is_white_figure()const;
    virtual ~Basic_figure() = default;
    Basic_figure(const Figures&, bool);
    virtual Figures what_figure()const;
    virtual void where_to_move(MoveMap& map,const std::array<std::array<std::unique_ptr<Basic_figure>,8>,8>& board
                               ,int current_i, int current_j,bool is_white_turn_to_move) = 0;
    virtual void handle_move();
};

using ArrayBoard = std::array<std::array<std::unique_ptr<Basic_figure>,8>,8>;
#endif // BASIC_FIGURE_H

