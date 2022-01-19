#include <gtest/gtest.h>
#include "io-ids/ioidkeeper.h"

TEST(TEST_ID_GENERATOR, INITIAL_NOT_MINE) {
    IoIdKeeper keeper;
    EXPECT_FALSE(keeper.isCurrAndDeactivateIf(0));
    EXPECT_FALSE(keeper.isCurrAndDeactivateIf(-1));
}

TEST(TEST_ID_GENERATOR, GET_FOLLOWS_SET) {
    IoIdKeeper keeper;
    int id = 42;
    keeper.setCurrent(id);
    EXPECT_EQ(keeper.getCurrent(), id);
    keeper.setCurrent(id+1);
    EXPECT_EQ(keeper.getCurrent(), id+1);
}

TEST(TEST_ID_GENERATOR, GET_CURRENT_TWICE) {
    IoIdKeeper keeper;
    int id = 42;
    keeper.setCurrent(id);
    EXPECT_EQ(keeper.getCurrent(), id);
    EXPECT_EQ(keeper.getCurrent(), id);
}

TEST(TEST_ID_GENERATOR, IS_CURRENT_ONCE_ONLY) {
    IoIdKeeper keeper;
    int id = 42;
    keeper.setCurrent(id);
    EXPECT_TRUE(keeper.isCurrAndDeactivateIf(id));
    EXPECT_FALSE(keeper.isCurrAndDeactivateIf(id));
}

TEST(TEST_ID_GENERATOR, IS_NOT_CURRENT) {
    IoIdKeeper keeper;
    int id = 42;
    keeper.setCurrent(id);
    EXPECT_FALSE(keeper.isCurrAndDeactivateIf(id+1));
    EXPECT_TRUE(keeper.isCurrAndDeactivateIf(id));
}

TEST(TEST_ID_GENERATOR, OVERWRITE) {
    IoIdKeeper keeper;
    int id = 42;
    keeper.setCurrent(id+1);
    EXPECT_FALSE(keeper.isCurrAndDeactivateIf(id));
    keeper.setCurrent(id);
    EXPECT_FALSE(keeper.isCurrAndDeactivateIf(id+1));
    EXPECT_TRUE(keeper.isCurrAndDeactivateIf(id));
}

TEST(TEST_ID_GENERATOR, NOT_ACTIVE_INIT) {
    IoIdKeeper keeper;
    EXPECT_FALSE(keeper.isActive());
}

TEST(TEST_ID_GENERATOR, SET_MAKES_ACTIVE) {
    IoIdKeeper keeper;
    keeper.setCurrent(42);
    EXPECT_TRUE(keeper.isActive());
}

TEST(TEST_ID_GENERATOR, MATCH_MAKES_INACTIVE) {
    IoIdKeeper keeper;
    int id = 42;
    keeper.setCurrent(id);
    keeper.isCurrAndDeactivateIf(id);
    EXPECT_FALSE(keeper.isActive());
}

TEST(TEST_ID_GENERATOR, MISS_KEEPS_ACTIVE) {
    IoIdKeeper keeper;
    int id = 42;
    keeper.setCurrent(id);
    keeper.isCurrAndDeactivateIf(id+1);
    EXPECT_TRUE(keeper.isActive());
}

TEST(TEST_ID_GENERATOR, DEACTIVATE) {
    IoIdKeeper keeper;
    keeper.setCurrent(42);
    keeper.deactivate();
    EXPECT_FALSE(keeper.isActive());
}
