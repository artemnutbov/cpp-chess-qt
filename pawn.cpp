#include "pawn.h"


Pawn::Pawn(const Figures& figure, bool is_white)
    :Basic_figure(figure, is_white),is_in_start_pos(true){}



void Pawn::handle_move() {
    if(is_in_start_pos)
        is_in_start_pos = false;
}


void Pawn::move_for_both_sides(MoveMap& map, const ArrayBoard& board,std::pair<int,int> new_index,bool is_white_pov) {
    bool is_go_up = is_white ?  is_white_pov : !is_white_pov;
    is_go_up ? --new_index.first : ++new_index.first;
    if(!board[new_index.first][new_index.second]) {
        if(new_index.first == 0 || new_index.first == 7)
            map.emplace(new_index, Move_types::promote_to_empty_square);
        else {
            map.emplace(new_index, Move_types::move_to_empty_square);
            is_go_up ? --new_index.first : ++new_index.first;
            if(is_in_start_pos)
                map.insert(IndexPair(new_index, Move_types::move_to_empty_square));
            is_go_up ? ++new_index.first : --new_index.first;
        }
    }


    ++new_index.second;
    if(board[new_index.first][new_index.second] && new_index.second < 8 ) {
        if(board[new_index.first][new_index.second]->is_white_figure() != is_white ) {
            if(new_index.first == 0 || new_index.first == 7)
                map.emplace(new_index, Move_types::promote_and_capture);
            else
                map.emplace(new_index, Move_types::capture);
        }
    }
    new_index.second-=2;
    if(board[new_index.first][new_index.second] && new_index.second >= 0  ) {
        if(board[new_index.first][new_index.second]->is_white_figure() != is_white ){
            if(new_index.first == 0 || new_index.first == 7)
                map.emplace(new_index, Move_types::promote_and_capture);
            else
                map.emplace(new_index, Move_types::capture);
        }
    }
}

void Pawn::where_to_move(MoveMap& map,const ArrayBoard& board
                         ,int current_i, int current_j,bool is_white_pov) {

    std::pair<int,int> new_index(current_i,current_j);

    move_for_both_sides(map,board,new_index,is_white_pov);
        // !!! need to handle en passant capture
        // !!! to black pawn too

}
