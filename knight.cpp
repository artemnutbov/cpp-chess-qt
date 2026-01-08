#include "knight.h"


Knight::Knight(const Figures& figure, bool is_white)
    :Basic_figure(figure,is_white) {}



void Knight::move_for_both_sides(MoveMap& map,const ArrayBoard& board,int current_row, int current_col) {
    std::pair<int,int> new_index(current_row, current_col);


    what_to_do_whith_figure(map, board, new_index, 1, 2);
    what_to_do_whith_figure(map, board, new_index, 2, 1);

    what_to_do_whith_figure(map, board, new_index, -1, -2);
    what_to_do_whith_figure(map, board, new_index, -2, -1);

    what_to_do_whith_figure(map, board, new_index, -1, 2);
    what_to_do_whith_figure(map, board, new_index, -2, 1);

    what_to_do_whith_figure(map, board, new_index, 2, -1);
    what_to_do_whith_figure(map, board, new_index, 1, -2);

}


void Knight::what_to_do_whith_figure(MoveMap& map, const ArrayBoard& board, std::pair<int,int> index, int pos_row, int pos_col) {

    std::function<void(int&,int)> func = [](int& i, int pos ) { i+=pos; };

    func(index.first, pos_row);
    func(index.second, pos_col);

    if(index.first < 8 && index.first >= 0 &&  index.second < 8 && index.second >= 0 ) {
        if(!board[index.first][index.second])
            map.insert(IndexPair(index, Move_types::move_to_empty_square));
        else if((board[index.first][index.second]->is_white != is_white) && (board[index.first][index.second]->what_figure() != Figures::white_king)
                 && (board[index.first][index.second]->what_figure() != Figures::black_king))  {
            map.insert(IndexPair(index, Move_types::capture));
        }

    }

}

void Knight::where_to_move(MoveMap& map, const ArrayBoard& board, int current_row, int current_col,bool is_white_turn_to_move) {

    if(is_white && is_white_turn_to_move) {
        move_for_both_sides(map, board, current_row, current_col);

    }
    else if(!is_white && !is_white_turn_to_move) {
        move_for_both_sides(map, board, current_row, current_col);
    }
}
