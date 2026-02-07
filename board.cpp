#include "board.h"
#include "pawn.h"
#include "queen.h"
#include "knight.h"
#include "king.h"
#include <QDebug>


Board::Board(): game_result_status(Game_Result_Status::Not_Started) {}
Figures Board::name_to_figure(Figures_Name name, bool is_white) {
    switch (name) {
    case Figures_Name::bishop:
        return is_white ? Figures::white_bishop : Figures::black_bishop;
    case Figures_Name::rook:
        return is_white ? Figures::white_rook : Figures::black_rook;
    case Figures_Name::knight:
        return is_white ? Figures::white_knight : Figures::black_knight;
    case Figures_Name::pawn:
        return is_white ? Figures::white_pawn : Figures::black_pawn;
    case Figures_Name::king:
        return is_white ? Figures::white_king : Figures::black_king;
    case Figures_Name::queen:
        return is_white ? Figures::white_queen : Figures::black_queen;
    default:
        break;
    }
    return is_white ? Figures::white_queen : Figures::black_queen;
}

void Board::set_up(bool white_pov){
    is_white_pov = white_pov;
    game_result_status = Game_Result_Status::Playing_Now;
    int white_pov_pieces_rank = is_white_pov ? 7 : 0;
    int black_pov_pieces_rank = !is_white_pov ? 7 : 0;

    int white_pov_pawns_rank = is_white_pov ? 6 : 1;
    int black_pov_pawns_rank = !is_white_pov ? 6 : 1;

    white_king_index = std::pair<int,int>(white_pov_pieces_rank,3);
    black_king_index = std::pair<int,int>(black_pov_pieces_rank,3);

    board[white_pov_pieces_rank][2] = std::make_unique<Bishop>(Figures::white_bishop,true);
    board[white_pov_pieces_rank][5] = std::make_unique<Bishop>(Figures::white_bishop,true);

    board[white_pov_pieces_rank][1] = std::make_unique<Knight>(Figures::white_knight,true);
    board[white_pov_pieces_rank][6] = std::make_unique<Knight>(Figures::white_knight,true);

    board[white_pov_pieces_rank][0] = std::make_unique<Rook>(Figures::white_rook,true);
    board[white_pov_pieces_rank][7] = std::make_unique<Rook>(Figures::white_rook,true);

    board[white_pov_pieces_rank][3] = std::make_unique<King>(Figures::white_king,true);
    board[white_pov_pieces_rank][4] = std::make_unique<Queen>(Figures::white_queen,true);



    board[black_pov_pieces_rank][0] = std::make_unique<Rook>(Figures::black_rook,false);
    board[black_pov_pieces_rank][7] = std::make_unique<Rook>(Figures::black_rook,false);

    board[black_pov_pieces_rank][2] = std::make_unique<Bishop>(Figures::black_bishop,false);
    board[black_pov_pieces_rank][5] = std::make_unique<Bishop>(Figures::black_bishop,false);

    board[black_pov_pieces_rank][1] = std::make_unique<Knight>(Figures::black_knight,false);
    board[black_pov_pieces_rank][6] = std::make_unique<Knight>(Figures::black_knight,false);

    board[black_pov_pieces_rank][3] = std::make_unique<King>(Figures::black_king,false);
    board[black_pov_pieces_rank][4] = std::make_unique<Queen>(Figures::black_queen,false);

    for(size_t i = 0;i<8;++i) {
        board[white_pov_pawns_rank][i] = std::make_unique<Pawn>(Figures::white_pawn,true);
        board[black_pov_pawns_rank][i] = std::make_unique<Pawn>(Figures::black_pawn,false);
    }
}

const std::pair<int,int>& Board::get_king_index(bool is_white)const {
    return is_white ? white_king_index : black_king_index;
}

bool Board::get_white_pov() const{
    return is_white_pov;
}
bool Board::valid_index(int i, int j) {
    if(board[i][j])
        return true;
    return  false;
}


Game_Result_Status Board::what_game_state()const {
    return game_result_status;
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
    if(board[y][x]->is_white_figure() != is_white_turn_to_move)
        return;
    board[y][x]->where_to_move(index_pair_map, board, y, x, is_white_pov);

    if((board[y][x]->what_figure() != Figures::black_king) && (board[y][x]->what_figure() != Figures::white_king)) {
        std::pair<int, int> king_index = (is_white_turn_to_move) ? white_king_index : black_king_index;
        for(const auto& it : index_pair_map) {
            if(it.second == Move_types::capture || it.second == Move_types::promote_and_capture){
                std::unique_ptr<Basic_figure> tmp = std::move(board[it.first.first][it.first.second]);
                std::swap(board[y][x], board[it.first.first][it.first.second]);
                bool is_in_check = static_cast<King*>(board[king_index.first][king_index.second].get())->is_king_under_attack(board, king_index.first, king_index.second, is_white_pov);
                std::swap(board[y][x], board[it.first.first][it.first.second]);
                board[it.first.first][it.first.second] = std::move(tmp);
                if(is_in_check)
                    index_pair_map.erase(it.first);
            }
            else {
                std::swap(board[y][x], board[it.first.first][it.first.second]);
                bool is_in_check = static_cast<King*>(board[king_index.first][king_index.second].get())->is_king_under_attack(board, king_index.first, king_index.second, is_white_pov);
                std::swap(board[y][x], board[it.first.first][it.first.second]);
                if(is_in_check)
                    index_pair_map.erase(it.first);

            }
        }
    }
}

void Board::set_result_state() {
    //  test try to implement checkmate
    if(!index_pair_map.empty())
        index_pair_map.clear();
    std::pair<int, int> king_index = (is_white_turn_to_move) ? white_king_index : black_king_index;
    bool is_in_check = static_cast<King*>(board[king_index.first][king_index.second].get())->is_king_under_attack(board, king_index.first, king_index.second, is_white_pov);


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

bool Board::is_pawn_promote(int x, int y) {
    Figures current_figure = board[y][x]->what_figure();
    if((current_figure == Figures::white_pawn || current_figure == Figures::black_pawn) && (y == 7 || y == 0)){
        board[y][x].reset();
        promote_pawn_index = *index_pair_map.find(std::pair<int,int>(y,x));
        return true;
    }
    return false;
}

const std::pair<int,int>& Board::get_promote_index() const{
    return promote_pawn_index.first;
}
void Board::promotion(Figures_Name figure_name, bool is_white) {
    Figures figure = name_to_figure(figure_name, is_white);
    if(figure_name == Figures_Name::bishop)
        board[promote_pawn_index.first.first][promote_pawn_index.first.second] = std::make_unique<Bishop>(figure, is_white);
    else if(figure_name == Figures_Name::knight)
        board[promote_pawn_index.first.first][promote_pawn_index.first.second] = std::make_unique<Knight>(figure, is_white);
    else if(figure_name == Figures_Name::rook)
        board[promote_pawn_index.first.first][promote_pawn_index.first.second] = std::make_unique<Rook>(figure, is_white);
    else if(figure_name == Figures_Name::queen)
        board[promote_pawn_index.first.first][promote_pawn_index.first.second] = std::make_unique<Queen>(figure, is_white);
}
bool Board::action_move(int x, int y, int new_x_index, int new_y_index) {
    auto it = index_pair_map.find(std::pair<int,int>(new_y_index,new_x_index));
    if(it == index_pair_map.end()) {
        return false;
    }
    if(it->second == Move_types::capture || it->second == Move_types::promote_and_capture)
        board[new_y_index][new_x_index].reset();
    else if(it->second == Move_types::short_castling) {

        board[y][0]->handle_move();

        std::swap(board[y][0], board[y][new_x_index+1]);
    }
    else if(it->second == Move_types::long_castling) {

        board[y][7]->handle_move();
        std::swap(board[y][7], board[y][new_x_index-1]);
    }

    //if(!board[new_y_index][new_x_index] && index_pair_map.contains(std::pair<int,int>(new_y_index,new_x_index))) {
    Figures current_figure = board[y][x]->what_figure();

    board[y][x]->handle_move();
    is_white_turn_to_move = !is_white_turn_to_move;
    if(current_figure == Figures::white_king) {
        white_king_index.first = new_y_index;
        white_king_index.second = new_x_index;

    }
    else if(current_figure == Figures::black_king){
        black_king_index.first = new_y_index;
        black_king_index.second = new_x_index;
    }
    std::swap(board[y][x],board[new_y_index][new_x_index]);

    //}
    return true;
}
