#include "pawn.h"


Pawn::Pawn(const Figures& figure,int x, int y, bool is_white)
    :Basic_figure(figure,x,y,is_white),is_in_start_pos(true) {}



void Pawn::handle_move(int x_pos, int y_pos) {
    if(is_in_start_pos)
        is_in_start_pos = false;
    x = x_pos;
    y = y_pos;
}

void Pawn::where_to_move(MoveMap& map,const ArrayBoard& board
                         ,int current_i, int current_j,bool is_white_turn_to_move) {

    std::pair<int,int> new_index(current_i,current_j);
    if(is_white && is_white_turn_to_move) {
        ++new_index.first;
        if(!board[new_index.first][new_index.second]) {
            map.insert(IndexPair(new_index, Move_types::move_to_empty_square));
            ++new_index.first;
            if(!board[new_index.first][new_index.second] && is_in_start_pos)
                map.insert(IndexPair(new_index, Move_types::move_to_empty_square));
            --new_index.first;
        }

        new_index.second+=1;
        if(board[new_index.first][new_index.second] && new_index.second < 8 ) {
            if(board[new_index.first][new_index.second]->is_white != is_white )
                map.insert(IndexPair(new_index, Move_types::capture));
        }
        new_index.second-=2;
        if(board[new_index.first][new_index.second] && new_index.second >= 0  ) {
            if(board[new_index.first][new_index.second]->is_white != is_white )
                map.insert(IndexPair(new_index, Move_types::capture));
        }
        // !!! need to handle last pos pawn(promote to any figure)
        // !!! and also need to handle en passant capture
        // !!! to black pawn too
    }
    else if(!is_white && !is_white_turn_to_move) { // need to write helper function that do all computatation
        --new_index.first;
        if(!board[new_index.first][new_index.second]) {
            map.insert(IndexPair(new_index, Move_types::move_to_empty_square));
            --new_index.first;
            if(!board[new_index.first][new_index.second] && is_in_start_pos)
                map.insert(IndexPair(new_index,  Move_types::move_to_empty_square));
            ++new_index.first;
        }

        ++new_index.second;
        if(board[new_index.first][new_index.second] && new_index.second < 8 ) {
            if(board[new_index.first][new_index.second]->is_white != is_white )
                map.insert(IndexPair(new_index, Move_types::capture));
        }
        new_index.second-=2;
        if(board[new_index.first][new_index.second] && new_index.second >= 0  ) {
            if(board[new_index.first][new_index.second]->is_white != is_white )
                map.insert(IndexPair(new_index, Move_types::capture));
        }
    }
}
