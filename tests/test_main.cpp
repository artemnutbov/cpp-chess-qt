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
