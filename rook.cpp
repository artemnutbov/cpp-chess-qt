#include "rook.h"

Rook::Rook(const Figures& figure,  bool is_white)
    :Basic_figure(figure, is_white), is_in_start_pos(true) {}

bool Rook::is_in_start_position()const {
    return is_in_start_pos;
}

void Rook::handle_move() {
    if(is_in_start_pos)
        is_in_start_pos = false;

}

void Rook::move_for_both_sides(MoveMap& map,const ArrayBoard& board,int current_i, int current_j) {
    std::pair<int,int> new_index(current_i, current_j);

    what_to_do_whith_figure(map, board, new_index, [](int i) { return i < 8; }, true, true);

    what_to_do_whith_figure(map, board, new_index, [](int i) { return i >= 0; }, false, true);

    what_to_do_whith_figure(map, board, new_index, [](int i) { return i < 8; }, true, false);

    what_to_do_whith_figure(map, board, new_index, [](int i) { return i >= 0; }, false, false);

}


template <typename Comp>
void Rook::what_to_do_whith_figure(MoveMap& map, const ArrayBoard& board,std::pair<int,int> index, Comp condition, bool is_increment, bool is_row) {
    int& ref_index = (is_row) ? index.first : index.second;
    std::function<void(int&)> func = (is_increment) ? [](int& i) { ++i; } :  [](int& i) { --i; };

    func(ref_index);
    while(condition(ref_index )) { // and need to check out Comp as const reference argument will be faster or no
        if(!board[index.first][index.second])
            map.insert(IndexPair(index, Move_types::move_to_empty_square));
        else if((board[index.first][index.second]->is_white_figure() != is_white) && (board[index.first][index.second]->what_figure() != Figures::white_king)
                                && (board[index.first][index.second]->what_figure() != Figures::black_king)) {
            map.insert(IndexPair(index, Move_types::capture));
            break;
        }
        else
            break;

        func(ref_index);
    }
}

void Rook::where_to_move(MoveMap& map, const ArrayBoard& board, int current_i, int current_j,bool ) {

        move_for_both_sides(map, board, current_i, current_j);
}
