#include <gtest/gtest.h>
#include "idkeeper.h"

TEST(TEST_ID_KEEPER_SINGLE, INITIAL_NOT_PENDING) {
    IdKeeperSingle keeper;
    EXPECT_FALSE(keeper.hasPending());
}

TEST(TEST_ID_KEEPER_SINGLE, GET_FOLLOWS_SET) {
    IdKeeperSingle keeper;
    int id = 42;
    keeper.setPending(id);
    EXPECT_EQ(keeper.getPending(), id);
    keeper.setPending(id+1);
    EXPECT_EQ(keeper.getPending(), id+1);
}

TEST(TEST_ID_KEEPER_SINGLE, GET_PENDING_ID_TWICE) {
    IdKeeperSingle keeper;
    int id = 42;
    keeper.setPending(id);
    EXPECT_EQ(keeper.getPending(), id);
    EXPECT_EQ(keeper.getPending(), id);
}

TEST(TEST_ID_KEEPER_SINGLE, IS_PENDING_ONCE_ONLY) {
    IdKeeperSingle keeper;
    int id = 42;
    keeper.setPending(id);
    EXPECT_TRUE(keeper.isPendingAndRemoveIf(id));
    EXPECT_FALSE(keeper.isPendingAndRemoveIf(id));
}

TEST(TEST_ID_KEEPER_SINGLE, IS_NOT_PENDING) {
    IdKeeperSingle keeper;
    int id = 42;
    keeper.setPending(id);
    EXPECT_FALSE(keeper.isPendingAndRemoveIf(id+1));
    EXPECT_TRUE(keeper.isPendingAndRemoveIf(id));
}

TEST(TEST_ID_KEEPER_SINGLE, OVERWRITE) {
    IdKeeperSingle keeper;
    int id = 42;
    keeper.setPending(id+1);
    EXPECT_FALSE(keeper.isPendingAndRemoveIf(id));
    keeper.setPending(id);
    EXPECT_FALSE(keeper.isPendingAndRemoveIf(id+1));
    EXPECT_TRUE(keeper.isPendingAndRemoveIf(id));
}

TEST(TEST_ID_KEEPER_SINGLE, NOT_PENDING_INIT) {
    IdKeeperSingle keeper;
    EXPECT_FALSE(keeper.hasPending());
}

TEST(TEST_ID_KEEPER_SINGLE, SET_MAKES_PENDING) {
    IdKeeperSingle keeper;
    keeper.setPending(42);
    EXPECT_TRUE(keeper.hasPending());
}

TEST(TEST_ID_KEEPER_SINGLE, MATCH_MAKES_NOT_PENDING) {
    IdKeeperSingle keeper;
    int id = 42;
    keeper.setPending(id);
    EXPECT_TRUE(keeper.isPendingAndRemoveIf(id));
    EXPECT_FALSE(keeper.hasPending());
}

TEST(TEST_ID_KEEPER_SINGLE, MISS_KEEPS_PENDING) {
    IdKeeperSingle keeper;
    int id = 42;
    keeper.setPending(id);
    keeper.isPendingAndRemoveIf(id+1);
    EXPECT_TRUE(keeper.hasPending());
}

TEST(TEST_ID_KEEPER_SINGLE, CLEAR) {
    IdKeeperSingle keeper;
    keeper.setPending(42);
    EXPECT_TRUE(keeper.hasPending());
    keeper.clear();
    EXPECT_FALSE(keeper.hasPending());
}
