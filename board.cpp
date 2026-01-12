#include "board.h"
#include "pawn.h"
#include "queen.h"
#include "knight.h"
#include "king.h"
#include <QDebug>

Board::Board():black_king_index(7, 3), white_king_index(0, 3),game_result_status(Game_Result_Status::Not_Started) {}

void Board::set_up(){
    game_result_status = Game_Result_Status::Playing_Now;
    // board[0][2] = std::make_unique<Bishop>(Figures::white_bishop,true);
    // board[0][5] = std::make_unique<Bishop>(Figures::white_bishop,true);

    // board[0][1] = std::make_unique<Knight>(Figures::white_knight,true);
    // board[0][6] = std::make_unique<Knight>(Figures::white_knight,true);
    // board[0][0] = std::make_unique<Rook>(Figures::white_rook,true);
    // board[0][7] = std::make_unique<Rook>(Figures::white_rook,true);
    board[0][3] = std::make_unique<King>(Figures::white_king,true);
    // board[0][4] = std::make_unique<Queen>(Figures::white_queen,true);

    // board[7][0] = std::make_unique<Rook>(Figures::black_rook,false);
    // board[7][7] = std::make_unique<Rook>(Figures::black_rook,false);



    //board[7][2] = std::make_unique<Bishop>(Figures::black_bishop,false);
    //board[7][5] = std::make_unique<Bishop>(Figures::black_bishop,false);

  //  board[7][1] = std::make_unique<Knight>(Figures::black_knight,false);
//    board[7][6] = std::make_unique<Knight>(Figures::black_knight,false);
    board[7][3] = std::make_unique<King>(Figures::black_king,false);
    board[7][4] = std::make_unique<Queen>(Figures::black_queen,false);

    for(size_t i = 0;i<8;++i) {
        //board[1][i] = std::make_unique<Pawn>(Figures::white_pawn,true);
        //board[6][i] = std::make_unique<Pawn>(Figures::black_pawn,false);
    }
}

bool Board::valid_index(int i, int j) {
    if(board[i][j])
        return true;
    return  false;
}


Figures Board::what_figure_index(int i, int j) {
    return board[i][j]->what_figure();
}

MoveMap& Board::all_legal_moves() {
    return index_pair_map;
}

void Board::all_figure_move(int x, int y) {
    if(!index_pair_map.empty())
        index_pair_map.clear();
    board[y][x]->where_to_move(index_pair_map, board, y, x, is_white_turn_to_move);

    if((board[y][x]->what_figure() != Figures::black_king) && (board[y][x]->what_figure() != Figures::white_king)) {
        std::pair<int, int> king_index = (is_white_turn_to_move) ? white_king_index : black_king_index;
        for(const auto& it : index_pair_map) {
            if(it.second == Move_types::capture){
                std::unique_ptr<Basic_figure> tmp = std::move(board[it.first.first][it.first.second]);
                std::swap(board[y][x], board[it.first.first][it.first.second]);
                bool is_in_check = static_cast<King*>(board[king_index.first][king_index.second].get())->is_king_under_attack(board, king_index.first, king_index.second);
                std::swap(board[y][x], board[it.first.first][it.first.second]);
                board[it.first.first][it.first.second] = std::move(tmp);
                if(is_in_check)
                    index_pair_map.erase(it.first);

            }
            else {
                std::swap(board[y][x], board[it.first.first][it.first.second]);
                bool is_in_check = static_cast<King*>(board[king_index.first][king_index.second].get())->is_king_under_attack(board, king_index.first, king_index.second);
                std::swap(board[y][x], board[it.first.first][it.first.second]);
                if(is_in_check)
                    index_pair_map.erase(it.first);

            }
        }
    }
}

void Board::action_move(int x, int y, int new_x_index, int new_y_index) {
    auto it = index_pair_map.find(std::pair<int,int>(new_y_index,new_x_index));
    if(it != index_pair_map.end()) {
        if(it->second == Move_types::capture)
            board[new_y_index][new_x_index].reset();
        else if(it->second == Move_types::short_castling) {

            board[y][0]->handle_move();

            std::swap(board[y][0], board[y][new_x_index+1]);
        }

        else if(it->second == Move_types::long_castling) {

            board[y][7]->handle_move();
            std::swap(board[y][7], board[y][new_x_index-1]);
        }
    }

    if(!board[new_y_index][new_x_index] && index_pair_map.contains(std::pair<int,int>(new_y_index,new_x_index))) {

        board[y][x]->handle_move();
        is_white_turn_to_move = !is_white_turn_to_move;
        if((board[y][x]->what_figure() == Figures::white_king)) {
            white_king_index.first = new_y_index;
            white_king_index.second = new_x_index;

        }
        else if((board[y][x]->what_figure() == Figures::black_king)){
            black_king_index.first = new_y_index;
            black_king_index.second = new_x_index;
        }
        std::swap(board[y][x],board[new_y_index][new_x_index]);
    }

    //  test try to implement checkmate
    if(!index_pair_map.empty())
        index_pair_map.clear();
    std::pair<int, int> king_index = (is_white_turn_to_move) ? white_king_index : black_king_index;
    bool is_in_check = static_cast<King*>(board[king_index.first][king_index.second].get())->is_king_under_attack(board, king_index.first, king_index.second);


    game_result_status = Game_Result_Status::Stalemate;
    for(size_t i = 0;i < 8;++i) {
        for(size_t j = 0;j < 8;++j){
            if(board[i][j] && board[i][j]->is_white_figure() == is_white_turn_to_move) {
                all_figure_move(j,i);
                if(!index_pair_map.empty()) {
                    game_result_status = Game_Result_Status::Playing_Now;
                    break;
                }
            }
        }
    }
    if(is_in_check && game_result_status ==  Game_Result_Status::Stalemate) {
        if(is_white_turn_to_move){
            game_result_status = Game_Result_Status::Black_win;
            qDebug() << "checkmate for white!!!!!!!";
        }
        else {
            game_result_status = Game_Result_Status::White_win;
            qDebug() << "checkmate for black!!!!!!!";

        }
    }
    else if(game_result_status ==  Game_Result_Status::Stalemate)
         qDebug() << "Stalemate !!!!!!!";

}
