#include <gtest/gtest.h>

#include "../board.h"

// simple test just to verify that testing work
TEST(BoardTest, InitialState) {
    Board board(true);
    EXPECT_EQ(board.Evaluate(), 0);
}

TEST(PerftTest, StartPositionDepth1) {
    Board board(true);

    EXPECT_EQ(board.Perft(1), 20);
    EXPECT_EQ(board.Perft(2), 400);
}

TEST(FENLoadTest, StartPositionMatchesDefault) {
    Board fen_board("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    Board std_board(true);

    EXPECT_EQ(fen_board.Evaluate(), std_board.Evaluate())
        << "FEN board evaluation does not match standard setup.";

    EXPECT_EQ(fen_board.Perft(2), std_board.Perft(2))
        << "FEN board move generation does not match standard setup.";
}

TEST(GameStateTest, DetectsWhiteCheckmate) {
    Board board("rnb1kbnr/pppp1ppp/8/4p3/6Pq/5P2/PPPPP2P/RNBQKBNR w KQkq - 1 3");

    board.SetResultState();

    EXPECT_EQ(board.GetGameState(), GameResultStatus::kBlackWin)
        << "Engine failed to recognize that White is in checkmate.";
}

TEST(GameStateTest, DetectsBlackCheckmate) {
    Board board("r1bqk1nr/pppp1Qpp/2n5/2b1p3/2B1P3/8/PPPP1PPP/RNB1K1NR b KQkq - 0 4");

    board.SetResultState();

    EXPECT_EQ(board.GetGameState(), GameResultStatus::kWhiteWin)
        << "Engine failed to recognize that Black is in checkmate.";
}
TEST(GameStateTest, DetectsStalemate) {
    Board board("k7/P7/1K6/8/8/8/8/8 b - - 0 1");

    board.SetResultState();

    EXPECT_EQ(board.GetGameState(), GameResultStatus::kStalemate)
        << "Engine failed to recognize a stalemate position.";
}

TEST(MoveTypeTest, IdentifiesEnPassant) {
    Board board("rnbqkbnr/ppp2ppp/8/3pP3/8/8/PPPP1PPP/RNBQKBNR w KQkq d6 0 2");

    auto moves = board.GenerateMoves();

    bool found_en_passant = false;

    for (const auto& move : moves) {
        if (move.from == 28 && move.to == 19) {
            EXPECT_EQ(move.type, MoveTypes::kEnPassant);
            found_en_passant = true;
        }
    }

    EXPECT_TRUE(found_en_passant) << "Did not generate or correctly tag kEnPassant.";
}
