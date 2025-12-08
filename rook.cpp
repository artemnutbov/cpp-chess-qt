#include "rook.h"

Rook::Rook(const Figures& figure,int x, int y, bool is_white)
    :Basic_figure(figure,x,y,is_white) {}



void Rook::handle_move() {

}

void Rook::move_for_both_sides(MoveMap& map,const ArrayBoard& board,int current_i, int current_j) {
    std::pair<int,int> new_index(current_i, current_j);

    what_to_do_whith_figure(map, board, new_index, [](int i) { return i < 8; }, true, new_index.first);
    new_index.first = current_i;


    what_to_do_whith_figure(map, board, new_index, [](int i) { return i >= 0; }, false, new_index.first);
    new_index.first = current_i;


    what_to_do_whith_figure(map, board, new_index, [](int i) { return i < 8; }, true, new_index.second);
    new_index.second = current_j;


    what_to_do_whith_figure(map, board, new_index, [](int i) { return i >= 0; }, false, new_index.second);
    new_index.second = current_j;

}


template <typename Comp>
void Rook::what_to_do_whith_figure(MoveMap& map, const ArrayBoard& board,const std::pair<int,int>& new_index, Comp condition, bool is_increment, int& index) {

    std::function<void(int&)> func = (is_increment) ? [](int& i) { ++i; } :  [](int& i) { --i; };



    func(index);
    while(condition(index )) { // and need to check out Comp as const reference argument will be faster or no
        if(!board[new_index.first][new_index.second])
            map.insert(IndexPair(new_index,false));
        else if(board[new_index.first][new_index.second]->is_white != is_white ) {
            map.insert(IndexPair(new_index,true));
            break;
        }
        else
            break;

        func(index);
    }

}

void Rook::where_to_move(MoveMap& map, const ArrayBoard& board, int current_i, int current_j,bool is_white_turn_to_move) {

    if(is_white && is_white_turn_to_move) {
        move_for_both_sides(map, board, current_i, current_j);

    }
    else if(!is_white && !is_white_turn_to_move) {
        move_for_both_sides(map, board, current_i, current_j);
    }
}
