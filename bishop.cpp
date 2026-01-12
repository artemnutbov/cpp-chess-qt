#include "bishop.h"

Bishop::Bishop(const Figures& figure, bool is_white)
    :Basic_figure(figure,is_white) {}




void Bishop::move_for_both_sides(MoveMap& map,const ArrayBoard& board,int current_row, int current_col) {
    std::pair<int,int> new_index(current_row, current_col);

    std::function<bool(int)> bound_right = [](int i) { return i < 8; };

    std::function<bool(int)> bound_left = [](int i) { return i >= 0; };

    what_to_do_whith_figure(map, board, new_index, bound_right, bound_right, true, true);

    what_to_do_whith_figure(map, board, new_index, bound_right, bound_left, true, false);

    what_to_do_whith_figure(map, board, new_index, bound_left, bound_right, false, true);

    what_to_do_whith_figure(map, board, new_index, bound_left, bound_left, false, false);

}


template <typename Comp>
void Bishop::what_to_do_whith_figure(MoveMap& map, const ArrayBoard& board, std::pair<int,int> index
                                     , const Comp& condition_row, const Comp& condition_col, bool is_increment_row, bool is_increment_col) {

    std::function<void(int&)> func_row = (is_increment_row) ? [](int& i) { ++i; }: [](int& i) { --i; } ;
    std::function<void(int&)> func_col = (is_increment_col) ? [](int& i) { ++i; }: [](int& i) { --i; } ;

    func_row(index.first);
    func_col(index.second);
    while(condition_row(index.first) &&  condition_col(index.second)) { // and need to check out Comp as const reference argument will be faster or no
        if(!board[index.first][index.second])
            map.insert(IndexPair(index,Move_types::move_to_empty_square));
        else if((board[index.first][index.second]->is_white_figure() != is_white) && (board[index.first][index.second]->what_figure() != Figures::white_king)
                                                                            && (board[index.first][index.second]->what_figure() != Figures::black_king) ) {
            map.insert(IndexPair(index,Move_types::capture));
            break;
        }
        else
            break;

        func_row(index.first);
        func_col(index.second);
    }

}

void Bishop::where_to_move(MoveMap& map, const ArrayBoard& board, int current_row, int current_col,bool is_white_turn_to_move) {

    if(is_white && is_white_turn_to_move) {
        move_for_both_sides(map, board, current_row, current_col);

    }
    else if(!is_white && !is_white_turn_to_move) {
        move_for_both_sides(map, board, current_row, current_col);
    }
}
