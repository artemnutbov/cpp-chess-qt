#include "board.h"
#include "pawn.h"
#include "queen.h"
#include "bishop.h"
#include "knight.h"
#include "rook.h"
#include "king.h"
#include <QDebug>
#include "random"

uint64_t Board::zobrist_pieces[12][64];
uint64_t Board::zobrist_side_to_move;
uint64_t Board::zobrist_castling[4];
uint64_t Board::zobrist_en_passant[8];


Board::Board(): game_result_status(Game_Result_Status::Not_Started) {
    board.fill(Figures::none);
    is_in_start_pos_board.fill(true);
}
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



void Board::init_zobrist() {
    // Use a fixed seed so the numbers are the same every time you run the game
    std::mt19937_64 rng(123456789);

    for (int p = 0; p < 12; ++p) {
        for (int s = 0; s < 64; ++s) {
            zobrist_pieces[p][s] = rng();
        }
    }
    zobrist_side_to_move = rng();
    for (int i = 0; i < 4; ++i) zobrist_castling[i] = rng();
    for (int i = 0; i < 8; ++i) zobrist_en_passant[i] = rng();
}

uint64_t Board::compute_hash() {
    uint64_t hash = 0;

    for (int i = 0; i < 64; ++i) {
        Figures fig = board[i];
        if (fig == Figures::none) continue;

        // Map enum to 0-11 index
        int piece_index = 0;
        switch(fig) {
        case Figures::white_pawn:   piece_index = 0; break;
        case Figures::white_knight: piece_index = 1; break;
        case Figures::white_bishop: piece_index = 2; break;
        case Figures::white_rook:   piece_index = 3; break;
        case Figures::white_queen:  piece_index = 4; break;
        case Figures::white_king:   piece_index = 5; break;
        case Figures::black_pawn:   piece_index = 6; break;
        case Figures::black_knight: piece_index = 7; break;
        case Figures::black_bishop: piece_index = 8; break;
        case Figures::black_rook:   piece_index = 9; break;
        case Figures::black_queen:  piece_index = 10; break;
        case Figures::black_king:   piece_index = 11; break;
        default: break;
        }
        hash ^= zobrist_pieces[piece_index][i];
    }

    if (!is_white_turn_to_move) {
        hash ^= zobrist_side_to_move;
    }

    // 3. Castling Rights (Based on your bool flag: true = has moved)
    // Assuming indices: 56=WR_A, 63=WR_H, 60=WK, 0=BR_A, 7=BR_H, 4=BK
    // Note: Adjust indices based on your POV logic if needed

    // White King Side (Rook H1 didn't move, King didn't move)

    if (board[63] == Figures::white_rook && !is_in_start_pos_board[63] &&
        board[60] == Figures::white_king && !is_in_start_pos_board[60]) hash ^= zobrist_castling[0];

    // White Queen Side
    if (board[56] == Figures::white_rook && !is_in_start_pos_board[56] &&
        board[60] == Figures::white_king && !is_in_start_pos_board[60]) hash ^= zobrist_castling[1];

    // Black King Side
    if (board[7] == Figures::black_rook && !is_in_start_pos_board[7] &&
        board[4] == Figures::black_king && !is_in_start_pos_board[4]) hash ^= zobrist_castling[2];

    // Black Queen Side
    if (board[0] == Figures::black_rook && !is_in_start_pos_board[0] &&
        board[4] == Figures::black_king && !is_in_start_pos_board[4]) hash ^= zobrist_castling[3];


    // 4. En Passant (Check history for double pawn push)
    if (!history.empty()) {
        const auto& last = history.back();
        // If last move was a double pawn push, add the FILE of that pawn
        if ((last.our_figure == Figures::white_pawn || last.our_figure == Figures::black_pawn) &&
            std::abs(last.from_square - last.to_square) == 16) {

            int file = last.to_square & 7;
            hash ^= zobrist_en_passant[file];
        }
    }

    return hash;
}

bool Board::is_repetition() const {
    int count = 0;

    // Iterate backwards through history
    for (auto it = history.rbegin(); it != history.rend(); ++it) {
        // If the hash matches the CURRENT board
        if (it->hash_snapshot == current_hash) {
            count++;
        }

        // We can stop checking if we hit a "irreversible" move
        if (it->move_type == Move_types::capture ||
            it->our_figure == Figures::white_pawn ||
            it->our_figure == Figures::black_pawn) {
            break;
        }
    }

    // Current state matches (1) + found in history (2) = 3 total
    return count >= 2;
}


void Board::set_up(bool white_pov){
    history.reserve(100); // reserve a little bigger than avg moves
    is_white_pov = white_pov;
    game_result_status = Game_Result_Status::Playing_Now;
    int white_pov_pieces_rank = is_white_pov ? 7 * 8  : 0;
    int black_pov_pieces_rank = !is_white_pov ? 7 * 8 : 0;

    int white_pov_pawns_rank = is_white_pov ? 6 * 8 : 1*8;
    int black_pov_pawns_rank = !is_white_pov ? 6 * 8 : 1*8;


    white_king_index = is_white_pov ? white_pov_pieces_rank + 4 : white_pov_pieces_rank + 3;
    black_king_index = is_white_pov ? black_pov_pieces_rank + 4 : black_pov_pieces_rank + 3;

    board[white_pov_pieces_rank + 2] = Figures::white_bishop;
    board[white_pov_pieces_rank + 5] = Figures::white_bishop;

    board[white_pov_pieces_rank + 1] = Figures::white_knight;
    board[white_pov_pieces_rank + 6] = Figures::white_knight;

    board[white_pov_pieces_rank + 0] = Figures::white_rook;
    board[white_pov_pieces_rank + 7] = Figures::white_rook;

    board[white_king_index] = Figures::white_king;
    board[is_white_pov ? white_pov_pieces_rank + 3 : white_pov_pieces_rank + 4] = Figures::white_queen;

    board[black_pov_pieces_rank + 0] = Figures::black_rook;
    board[black_pov_pieces_rank + 7] = Figures::black_rook;

    board[black_pov_pieces_rank + 2] = Figures::black_bishop;
    board[black_pov_pieces_rank + 5] = Figures::black_bishop;

    board[black_pov_pieces_rank + 1] = Figures::black_knight;
    board[black_pov_pieces_rank + 6] = Figures::black_knight;

    board[black_king_index] = Figures::black_king;
    board[is_white_pov ? black_pov_pieces_rank + 3 : black_pov_pieces_rank + 4] = Figures::black_queen;

    for(size_t i = 0;i<8;++i) {
        board[white_pov_pawns_rank + i] = Figures::white_pawn;
        board[black_pov_pawns_rank + i] = Figures::black_pawn;
    }

    current_hash = compute_hash();
}

int Board::get_king_index(bool is_white)const {
    return is_white ? white_king_index : black_king_index;
}

bool Board::get_white_pov() const{
    return is_white_pov;
}
bool Board::valid_index(int i) {
    if(board[i] != Figures::none)
        return true;
    return  false;
}


Game_Result_Status Board::what_game_state()const {
    return game_result_status;
}

Figures Board::what_figure_index(int i) {
    return board[i];
}

MoveMap& Board::all_legal_moves() {
    return index_pair_map;
}


void Board::square_move(int start_square) {
    Figures current_name = board[start_square];
    if(current_name == Figures::black_bishop || current_name == Figures::white_bishop)
        Bishop::get_bishop_moves(index_pair_map, board, start_square);
    else if(current_name == Figures::black_pawn || current_name == Figures::white_pawn) {
        bool is_en_passant = false;
        MoveInfo last_move = *std::prev(history.end());
        if((last_move.our_figure ==  Figures::black_pawn || last_move.our_figure == Figures::white_pawn)){
            int distance = abs((last_move.from_square >> 3) - (last_move.to_square >> 3));
            is_en_passant = distance == 2 ? true : false;
        }
        Pawn::get_pawn_moves(index_pair_map, board, is_in_start_pos_board, start_square, last_move.to_square , is_white_pov, is_en_passant);
    }
    else if(current_name == Figures::black_knight || current_name == Figures::white_knight)
        Knight::get_knight_moves(index_pair_map, board, start_square);
    else if(current_name == Figures::black_rook || current_name == Figures::white_rook)
        Rook::get_rook_moves(index_pair_map, board, start_square);
    else if(current_name == Figures::black_queen || current_name == Figures::white_queen)
        Queen::get_queen_moves(index_pair_map, board, start_square);
    else  // Figures::none never appears here
        King::get_king_moves(index_pair_map, board, is_in_start_pos_board, start_square, is_white_pov,BasicFigure::figure_to_side(board[start_square]));
}

void Board::all_figure_move(int start_square) {
    if(!index_pair_map.empty())
        index_pair_map.clear();
    if(BasicFigure::figure_to_side(board[start_square]) != is_white_turn_to_move)
        return;
    square_move(start_square);

    if((board[start_square] != Figures::black_king) && (board[start_square] != Figures::white_king)) {
        int king_index = (is_white_turn_to_move) ? white_king_index : black_king_index;
        for(const auto& it : index_pair_map) {
            if(it.second == Move_types::capture || it.second == Move_types::promote_and_capture){
                Figures tmp = board[it.first];
                board[it.first] = Figures::none;

                std::swap(board[start_square], board[it.first]);
                bool is_in_check = King::is_king_under_attack(board, king_index, is_white_pov, BasicFigure::figure_to_side(board[king_index]));

                std::swap(board[start_square], board[it.first]);
                board[it.first] = tmp;
                if(is_in_check)
                    index_pair_map.erase(it.first);
            }
            else {
                std::swap(board[start_square], board[it.first]);
                bool is_in_check = King::is_king_under_attack(board, king_index, is_white_pov, BasicFigure::figure_to_side(board[king_index]));
                std::swap(board[start_square], board[it.first]);
                if(is_in_check)
                    index_pair_map.erase(it.first);
            }
        }
    }
}

void Board::set_result_state() {
    if(!index_pair_map.empty())
        index_pair_map.clear();
    int king_index = (is_white_turn_to_move) ? white_king_index : black_king_index;
    bool is_in_check = King::is_king_under_attack(board, king_index, is_white_pov, BasicFigure::figure_to_side(board[king_index]));

    game_result_status = Game_Result_Status::Stalemate;
    for(size_t i = 0;i < 64;++i) {
        if(board[i] != Figures::none && BasicFigure::figure_to_side(board[i]) == is_white_turn_to_move) {
            all_figure_move(i);
            if(!index_pair_map.empty()) {
                game_result_status = Game_Result_Status::Playing_Now;
                break;
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
    else if(game_result_status == Game_Result_Status::Stalemate)
        qDebug() << "Stalemate !!!!!!!";
    else if (is_repetition()) {
        game_result_status = Game_Result_Status::Stalemate; // or create a Draw_Repetition status
        qDebug() << "Draw by Repetition!";
        return;
    }
    else  { // 100 bsc 50 for white and 50 for black
        auto penultimate_it = std::prev(history.end());
        if(penultimate_it->additional_figure != Figures::none || penultimate_it->our_figure == Figures::white_pawn || penultimate_it->our_figure == Figures::black_pawn) {
            count_50rule_draw = 0;
        }
        if(count_50rule_draw == 100) {
            game_result_status = Game_Result_Status::Stalemate; // or create a Draw_50rules status
            return;
        }
    }


}

bool Board::is_pawn_promote(int i) {

    Figures current_figure = board[i];
    if((current_figure == Figures::white_pawn || current_figure == Figures::black_pawn) && ((i >> 3 )== 7 || (i >> 3 ) == 0)){
        board[i] = Figures::none;
        promote_pawn_index = *index_pair_map.find(i);
        return true;
    }
    return false;
}

int Board::get_promote_index() const{
    return promote_pawn_index.first;
}
void Board::promotion(Figures_Name figure_name, bool is_white) {
    Figures figure = name_to_figure(figure_name, is_white);
    board[promote_pawn_index.first] = figure;
}
bool Board::action_move(int start_square, int new_square) {
    auto it = index_pair_map.find(new_square);
    if(it == index_pair_map.end()) {
        return false;
    }
    MoveInfo current_move;
    current_move.from_square = start_square;
    current_move.to_square = new_square;
    current_move.our_figure = board[start_square];
    current_move.move_type = it->second;
    current_move.hash_snapshot = current_hash;

    is_in_start_pos_board[new_square] = false;

    if(it->second == Move_types::capture || it->second == Move_types::promote_and_capture){
        current_move.additional_figure = board[new_square];
        board[new_square] = Figures::none;
    }
    else if(it->second == Move_types::short_castling) {
        //board[start_square >> 3) * 8].second = false;
        if(is_white_pov)
            std::swap(board[(start_square >> 3) * 8 + 7], board[(start_square >> 3) * 8 + (new_square & 7) - 1 ]);
        else
            std::swap(board[(start_square >> 3) * 8], board[(start_square >> 3) * 8 + (new_square & 7) + 1]);
    }
    else if(it->second == Move_types::long_castling) {
        //board[y][7]->handle_move();
        if(is_white_pov)
            std::swap(board[(start_square >> 3) * 8], board[(start_square >> 3) * 8 + (new_square & 7) + 1]);
        else
            std::swap(board[(start_square >> 3) * 8 + 7], board[(start_square >> 3) * 8 + (new_square & 7) - 1 ]);
    }
    else if(it->second == Move_types::en_passant) {
        if((new_square >> 3) == 2){
            current_move.additional_figure = board[new_square + 8];
            board[new_square + 8] = Figures::none;
        }
        else {
            current_move.additional_figure = board[new_square - 8];
            board[new_square - 8] = Figures::none;
        }
    }
    is_white_turn_to_move = !is_white_turn_to_move;
    ++count_50rule_draw;

    if(current_move.our_figure == Figures::white_king) {
        white_king_index = new_square;
    }
    else if(current_move.our_figure == Figures::black_king){
        black_king_index = new_square;
    }
    board[new_square] = board[start_square];
    board[start_square] = Figures::none;

    history.push_back(current_move);
    current_hash = compute_hash();
    return true;
}


void Board::undo_move() {
    if(history.empty())
        return;
    MoveInfo last_move = history.back();
    std::swap(board[last_move.to_square],board[last_move.from_square]);
    if(last_move.move_type ==  Move_types::capture || last_move.move_type == Move_types::promote_and_capture )
        board[last_move.to_square] = last_move.additional_figure;
    else if(last_move.move_type == Move_types::short_castling) {
        if(is_white_pov)
            std::swap(board[(last_move.from_square >> 3) * 8 + 7], board[(last_move.from_square >> 3) * 8 + (last_move.to_square & 7) - 1 ]);
        else
            std::swap(board[(last_move.from_square >> 3) * 8], board[(last_move.from_square >> 3) * 8 + (last_move.to_square & 7) + 1]);
    }
    else if(last_move.move_type == Move_types::long_castling) {
        if(is_white_pov)
            std::swap(board[(last_move.from_square >> 3) * 8], board[(last_move.from_square >> 3) * 8 + (last_move.to_square & 7) + 1]);
        else
            std::swap(board[(last_move.from_square >> 3) * 8 + 7], board[(last_move.from_square >> 3) * 8 + (last_move.to_square & 7) - 1 ]);
    }
    else if(last_move.move_type == Move_types::en_passant) {
        if((last_move.to_square >> 3) == 2)
            board[last_move.to_square + 8] = last_move.additional_figure;
        else
            board[last_move.to_square - 8] = last_move.additional_figure;
    }
    if(last_move.move_type == Move_types::move_to_empty_square || last_move.move_type == Move_types::promote_and_capture) {
        board[last_move.from_square] = last_move.our_figure;
    }

    if(last_move.our_figure == Figures::white_king) {
        white_king_index = last_move.from_square;

    }
    else if(last_move.our_figure == Figures::black_king){
        black_king_index = last_move.from_square;
    }

    is_white_turn_to_move = !is_white_turn_to_move;

    // check if pawn start pos match start game pawn po
    if(last_move.our_figure == Figures::black_pawn || last_move.our_figure == Figures::white_pawn) {
        bool is_white = BasicFigure::figure_to_side(last_move.our_figure);
        bool is_go_up = is_white ?  is_white_pov : !is_white_pov;
        if(is_go_up && (last_move.from_square >> 3) == 6 || !is_go_up && (last_move.from_square >> 3) == 1 )
            is_in_start_pos_board[last_move.from_square] = true;
    }
    else
        is_in_start_pos_board[last_move.from_square] = true;
    current_hash = last_move.hash_snapshot;
    history.pop_back();
}

