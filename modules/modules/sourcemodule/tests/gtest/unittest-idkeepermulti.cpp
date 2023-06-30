#include <gtest/gtest.h>
#include "idkeeper.h"

TEST(TEST_ID_KEEPER_MULTI, INITIAL_NOT_PENDING) {
    IdKeeperMulti keeper;
    EXPECT_FALSE(keeper.hasPending());
}

TEST(TEST_ID_KEEPER_MULTI, IS_PENDING_ONCE_ONLY) {
    IdKeeperMulti keeper;
    int id = 42;
    keeper.setPending(id);
    EXPECT_TRUE(keeper.isPendingAndRemoveIf(id));
    EXPECT_FALSE(keeper.isPendingAndRemoveIf(id));
}

TEST(TEST_ID_KEEPER_MULTI, IS_NOT_PENDING) {
    IdKeeperMulti keeper;
    int id = 42;
    keeper.setPending(id);
    EXPECT_FALSE(keeper.isPendingAndRemoveIf(id+1));
    EXPECT_TRUE(keeper.isPendingAndRemoveIf(id));
}

TEST(TEST_ID_KEEPER_MULTI, MULTIPLE_ADD_REMOVE) {
    IdKeeperMulti keeper;
    int id = 42;
    keeper.setPending(id);
    EXPECT_TRUE(keeper.hasPending());
    keeper.setPending(id+1);
    EXPECT_TRUE(keeper.hasPending());

    EXPECT_TRUE(keeper.isPendingAndRemoveIf(id));
    EXPECT_TRUE(keeper.hasPending());
    EXPECT_TRUE(keeper.isPendingAndRemoveIf(id+1));
    EXPECT_FALSE(keeper.hasPending());

    EXPECT_FALSE(keeper.isPendingAndRemoveIf(id));
    EXPECT_FALSE(keeper.hasPending());
    EXPECT_FALSE(keeper.isPendingAndRemoveIf(id+1));
    EXPECT_FALSE(keeper.hasPending());
}

TEST(TEST_ID_KEEPER_MULTI, NOT_PENDING_INIT) {
    IdKeeperMulti keeper;
    EXPECT_FALSE(keeper.hasPending());
}

TEST(TEST_ID_KEEPER_MULTI, SET_MAKES_PENDING) {
    IdKeeperMulti keeper;
    keeper.setPending(42);
    EXPECT_TRUE(keeper.hasPending());
}

TEST(TEST_ID_KEEPER_MULTI, MATCH_MAKES_NOT_PENDING) {
    IdKeeperMulti keeper;
    int id = 42;
    keeper.setPending(id);
    EXPECT_TRUE(keeper.isPendingAndRemoveIf(id));
    EXPECT_FALSE(keeper.hasPending());
}

TEST(TEST_ID_KEEPER_MULTI, MISS_KEEPS_PENDING) {
    IdKeeperMulti keeper;
    int id = 42;
    keeper.setPending(id);
    EXPECT_FALSE(keeper.isPendingAndRemoveIf(id+1));
    EXPECT_TRUE(keeper.hasPending());
}

TEST(TEST_ID_KEEPER_MULTI, CLEAR) {
    IdKeeperMulti keeper;
    keeper.setPending(42);
    keeper.setPending(43);
    EXPECT_TRUE(keeper.hasPending());
    keeper.clear();
    EXPECT_FALSE(keeper.hasPending());
}
