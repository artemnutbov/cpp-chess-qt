#include "king.h"
#include "rook.h"

King::King(const Figures& figure, bool is_white)
    :Basic_figure(figure,is_white), is_in_start_pos(true) {}


void King::handle_move() {
    if(is_in_start_pos)
        is_in_start_pos = false;
}


bool King::is_king_under_attack(const ArrayBoard& board, int current_row, int current_col) { // need to rewrite bcs it is awful to take 2 int instead of  std::pair
    if(is_attacked_by_knight(board, current_row, current_col) || is_attacked_by_bishop(board, current_row, current_col)
        || is_attacked_by_rook(board, current_row, current_col) || is_attacked_by_pawn(board, current_row, current_col) || is_enemy_king_close(board,  current_row, current_col))
        return true;
    return false;
}

bool King::is_attacked_by_pawn(const ArrayBoard& board,int current_row, int current_col) {
    std::pair<int,int> new_index(current_row, current_col);

    if(where_is_pawn(board, new_index, true) || where_is_pawn(board, new_index,  false) )
        return true;

    return false;
}

bool King::where_is_pawn(const ArrayBoard& board, std::pair<int,int> index, bool is_left_attack) {
    index.second += (is_left_attack) ? 1 : -1;
    index.first += (is_white) ?  1  : -1;

    if(index.first < 8 && index.first >= 0 &&  index.second < 8 && index.second >= 0 ) {
        if(!board[index.first][index.second])
            return false;
        else if(board[index.first][index.second]->is_white_figure() != is_white &&
                 (board[index.first][index.second]->what_figure() == Figures::white_pawn ||  board[index.first][index.second]->what_figure() == Figures::black_pawn)) {
            return true;
        }

    }
    return false;

}

bool King::is_attacked_by_knight(const ArrayBoard& board,int current_row, int current_col) {
    std::pair<int,int> new_index(current_row, current_col);

    if(where_is_knight(board, new_index, 1, 2) || where_is_knight(board, new_index, 2, 1)
        || where_is_knight(board, new_index, -1, -2) || where_is_knight(board, new_index, -2, -1)
        || where_is_knight(board, new_index, -1, 2) || where_is_knight(board, new_index, -2, 1)
        || where_is_knight(board, new_index, 2, -1) || where_is_knight(board, new_index, 1, -2))
        return true;

    return false;
}



bool King::where_is_knight(const ArrayBoard& board, std::pair<int,int> index, int pos_row, int pos_col) {

    std::function<void(int&,int)> func = [](int& i, int pos ) { i+=pos; };

    func(index.first, pos_row);
    func(index.second, pos_col);

    if(index.first < 8 && index.first >= 0 &&  index.second < 8 && index.second >= 0 ) {
        if(!board[index.first][index.second])
            return false;
        else if(board[index.first][index.second]->is_white_figure() != is_white &&
                 (board[index.first][index.second]->what_figure() == Figures::white_knight ||  board[index.first][index.second]->what_figure() == Figures::black_knight)) {
            return true;
        }

    }
    return false;

}

bool King::is_attacked_by_bishop(const ArrayBoard& board,int current_row, int current_col) {
    std::pair<int,int> new_index(current_row, current_col);

    std::function<bool(int)> bound_right = [](int i) { return i < 8; };

    std::function<bool(int)> bound_left = [](int i) { return i >= 0; };

    if(where_is_bishop(board, new_index, bound_right, bound_right, true, true) || where_is_bishop(board, new_index, bound_right, bound_left, true, false)
        || where_is_bishop(board, new_index, bound_left, bound_right, false, true) || where_is_bishop(board, new_index, bound_left, bound_left, false, false))
        return true;
    return false;
}


template <typename Comp>
bool King::where_is_bishop(const ArrayBoard& board, std::pair<int,int> index
                                     ,const Comp& condition_row, const Comp& condition_col, bool is_increment_row, bool is_increment_col) {

    std::function<void(int&)> func_row = (is_increment_row) ? [](int& i) { ++i; }: [](int& i) { --i; } ;
    std::function<void(int&)> func_col = (is_increment_col) ? [](int& i) { ++i; }: [](int& i) { --i; } ;

    func_row(index.first);
    func_col(index.second);
    while(condition_row(index.first) &&  condition_col(index.second)) { // and need to check out Comp as const reference argument will be faster or no
        if(!board[index.first][index.second]){
            func_row(index.first);
            func_col(index.second);
            continue;
        }
        Figures is_attacked_by = board[index.first][index.second]->what_figure();
        if(is_white) {
            if(is_attacked_by == Figures::white_king) {
                func_row(index.first);
                func_col(index.second);
                continue;
            }
        }
        else{
            if(is_attacked_by == Figures::black_king) {
                func_row(index.first);
                func_col(index.second);
                continue;
            }
        }


        if(board[index.first][index.second]->is_white_figure() != is_white && (is_attacked_by == Figures::white_bishop || is_attacked_by == Figures::black_bishop
                                                                       || is_attacked_by == Figures::black_queen || is_attacked_by == Figures::white_queen )) {
            return true;
        }
        else
            return false;

        func_row(index.first);
        func_col(index.second);
    }
    return false;
}


bool King::is_attacked_by_rook(const ArrayBoard& board,int current_i, int current_j) {
    std::pair<int,int> new_index(current_i, current_j);
    if( where_is_rook(board, new_index, [](int i) { return i < 8; }, true, true) || where_is_rook(board, new_index, [](int i) { return i >= 0; }, false, true)
        || where_is_rook(board, new_index, [](int i) { return i < 8; }, true, false) || where_is_rook(board, new_index, [](int i) { return i >= 0; }, false, false))
        return true;
    return false;
}


template <typename Comp>
bool King::where_is_rook(const ArrayBoard& board,std::pair<int,int> new_index, Comp condition, bool is_increment, bool is_row) {
    int& index = (is_row) ? new_index.first : new_index.second;
    std::function<void(int&)> func = (is_increment) ? [](int& i) { ++i; } :  [](int& i) { --i; };

    func(index);
    while(condition(index )) { // and need to check out Comp as const reference argument will be faster or no
        if(!board[new_index.first][new_index.second]) {
            func(index);
            continue;
        }
        Figures is_attacked_by = board[new_index.first][new_index.second]->what_figure();
        if(is_white) {
            if(is_attacked_by == Figures::white_king) {
                func(index);
                continue;
            }
        }
        else{
            if(is_attacked_by == Figures::black_king) {
                func(index);
                continue;
            }
        }
        if(board[new_index.first][new_index.second]->is_white_figure() != is_white && (is_attacked_by == Figures::white_rook || is_attacked_by == Figures::black_rook
            || is_attacked_by == Figures::black_queen || is_attacked_by == Figures::white_queen)) {
            return true;
        }
        else
            return false;
        func(index);
    }
    return false;
}

bool King::is_enemy_king_close(const ArrayBoard& board, int current_row, int current_col) {
    for(int i = -1; i < 2;++i) {
        for(int j = -1; j < 2;++j) {
            if(i == 0 && j == 0)
                continue;
            std::pair<int,int> index(current_row+i, current_col+j);
            if(index.first < 8 && index.first >= 0 &&  index.second < 8 && index.second >= 0 ) {
                Figures enemy_king = is_white ? Figures::black_king : Figures::white_king;
                if(board[index.first][index.second] && board[index.first][index.second]->what_figure() == enemy_king )
                    return true;

            }
        }
    }

    return false;
}

void King::move_for_both_sides(MoveMap& map, const ArrayBoard& board, int current_row, int current_col) {
    for(int i = -1; i < 2;++i) {
        for(int j = -1; j < 2;++j) {
            if(i == 0 && j == 0)
                continue;
            std::pair<int,int> index(current_row+i, current_col+j);
            if(index.first < 8 && index.first >= 0 &&  index.second < 8 && index.second >= 0 ) {
                if(board[index.first][index.second] && board[index.first][index.second]->is_white_figure() == is_white )
                    continue;
                bool can_move = is_king_under_attack(board, index.first, index.second);

                if(!board[index.first][index.second]){
                    if(!can_move)
                        map.insert(IndexPair(index, Move_types::move_to_empty_square));
                    continue;
                }
                else if(board[index.first][index.second]->is_white_figure() != is_white && !can_move ) {
                    map.insert(IndexPair(index, Move_types::capture));
                }
            }
        }
    }
    Figures rook = (is_white) ? Figures::white_rook : Figures::black_rook;



    if(is_in_start_pos && board[current_row][0] && board[current_row][0]->what_figure() == rook) {
        if(!board[current_row][current_col - 1] && !board[current_row][current_col - 2]) {
            if(dynamic_cast<Rook*>(board[current_row][0].get())->is_in_start_position() && !is_king_under_attack(board, current_row, current_col)
                &&  !is_king_under_attack(board, current_row, current_col - 1) && !is_king_under_attack(board, current_row, current_col - 2)) {
                map.insert(IndexPair(std::pair<int,int>(current_row, current_col - 2), Move_types::short_castling));
            }
        }
    }

    if(is_in_start_pos && board[current_row][7] && board[current_row][7]->what_figure() == rook) {
        if(!board[current_row][current_col + 1] && !board[current_row][current_col + 2] && !board[current_row][current_col + 3]) {
            if(dynamic_cast<Rook*>(board[current_row][0].get())->is_in_start_position() && !is_king_under_attack(board, current_row, current_col)
                &&  !is_king_under_attack(board, current_row, current_col + 1) && !is_king_under_attack(board, current_row, current_col + 2)
                && !is_king_under_attack(board, current_row, current_col + 3)) {
                map.insert(IndexPair(std::pair<int,int>(current_row, current_col + 2), Move_types::long_castling));
            }
        }
    }

}
void King::where_to_move(MoveMap& map, const ArrayBoard& board, int current_row, int current_col, bool is_white_turn_to_move) {

    if(is_white && is_white_turn_to_move) {
        move_for_both_sides(map, board, current_row, current_col);
    }
    else if(!is_white && !is_white_turn_to_move) {
        move_for_both_sides(map, board, current_row, current_col);
    }
}
