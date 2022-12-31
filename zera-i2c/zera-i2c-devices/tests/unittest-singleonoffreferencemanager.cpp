#include "singleonoffreferencemanager.h"
#include <gtest/gtest.h>

TEST(TEST_SINGLE_ON_OFF_MANAG, OneReferenceInitialState) {
    SingleOnOffReferenceManager<QString> manag;
    EXPECT_FALSE(manag.isFirstRef("foo"));
    EXPECT_TRUE(manag.hasNoRefs("foo"));
}

TEST(TEST_SINGLE_ON_OFF_MANAG, OneReferenceFirstState) {
    SingleOnOffReferenceManager<QString> manag;
    manag.addRef("foo");
    EXPECT_TRUE(manag.isFirstRef("foo"));
    EXPECT_FALSE(manag.hasNoRefs("foo"));
}

TEST(TEST_SINGLE_ON_OFF_MANAG, OneReferenceSecondState) {
    SingleOnOffReferenceManager<QString> manag;
    manag.addRef("foo");
    manag.addRef("foo");
    EXPECT_FALSE(manag.isFirstRef("foo"));
    EXPECT_FALSE(manag.hasNoRefs("foo"));
}

TEST(TEST_SINGLE_ON_OFF_MANAG, OneReferenceTwoOnOff) {
    SingleOnOffReferenceManager<QString> manag;
    manag.addRef("foo");
    manag.addRef("foo");
    EXPECT_FALSE(manag.isFirstRef("foo"));
    EXPECT_FALSE(manag.hasNoRefs("foo"));
    manag.freeRef("foo");
    EXPECT_TRUE(manag.isFirstRef("foo"));
    EXPECT_FALSE(manag.hasNoRefs("foo"));
    manag.freeRef("foo");
    EXPECT_FALSE(manag.isFirstRef("foo"));
    EXPECT_TRUE(manag.hasNoRefs("foo"));
}

TEST(TEST_SINGLE_ON_OFF_MANAG, OneReferenceOnOffState) {
    SingleOnOffReferenceManager<QString> manag;
    manag.addRef("foo");
    manag.freeRef("foo");
    EXPECT_FALSE(manag.isFirstRef("foo"));
    EXPECT_TRUE(manag.hasNoRefs("foo"));

}

TEST(TEST_SINGLE_ON_OFF_MANAG, OneReferenceUnderflowState) {
    SingleOnOffReferenceManager<QString> manag;
    manag.freeRef("foo");
    EXPECT_FALSE(manag.isFirstRef("foo"));
    EXPECT_TRUE(manag.hasNoRefs("foo"));
}


TEST(TEST_SINGLE_ON_OFF_MANAG, TwoReferenceInitialState) {
    SingleOnOffReferenceManager<QString> manag;
    EXPECT_FALSE(manag.isFirstRef("foo"));
    EXPECT_TRUE(manag.hasNoRefs("foo"));
    EXPECT_FALSE(manag.isFirstRef("bar"));
    EXPECT_TRUE(manag.hasNoRefs("bar"));
}

TEST(TEST_SINGLE_ON_OFF_MANAG, TwoReferenceFirstState) {
    SingleOnOffReferenceManager<QString> manag;
    manag.addRef("foo");
    manag.addRef("bar");
    EXPECT_TRUE(manag.isFirstRef("foo"));
    EXPECT_FALSE(manag.hasNoRefs("foo"));
    EXPECT_TRUE(manag.isFirstRef("bar"));
    EXPECT_FALSE(manag.hasNoRefs("bar"));
}

TEST(TEST_SINGLE_ON_OFF_MANAG, TwoReferenceOnOffState) {
    SingleOnOffReferenceManager<QString> manag;
    manag.addRef("foo");
    manag.freeRef("foo");
    EXPECT_FALSE(manag.isFirstRef("foo"));
    EXPECT_TRUE(manag.hasNoRefs("foo"));
    EXPECT_FALSE(manag.isFirstRef("bar"));
    EXPECT_TRUE(manag.hasNoRefs("bar"));
    manag.addRef("bar");
    manag.freeRef("bar");
    EXPECT_FALSE(manag.isFirstRef("foo"));
    EXPECT_TRUE(manag.hasNoRefs("foo"));
    EXPECT_FALSE(manag.isFirstRef("bar"));
    EXPECT_TRUE(manag.hasNoRefs("bar"));
}

TEST(TEST_SINGLE_ON_OFF_MANAG, TwoReferenceNoCrossTalk) {
    SingleOnOffReferenceManager<QString> manag;
    manag.addRef("foo");
    EXPECT_TRUE(manag.isFirstRef("foo"));
    EXPECT_FALSE(manag.hasNoRefs("foo"));
    EXPECT_FALSE(manag.isFirstRef("bar"));
    EXPECT_TRUE(manag.hasNoRefs("bar"));
    manag.freeRef("foo");
    EXPECT_FALSE(manag.isFirstRef("foo"));
    EXPECT_TRUE(manag.hasNoRefs("foo"));
    EXPECT_FALSE(manag.isFirstRef("bar"));
    EXPECT_TRUE(manag.hasNoRefs("bar"));

    manag.addRef("bar");
    EXPECT_FALSE(manag.isFirstRef("foo"));
    EXPECT_TRUE(manag.hasNoRefs("foo"));
    EXPECT_TRUE(manag.isFirstRef("bar"));
    EXPECT_FALSE(manag.hasNoRefs("bar"));
    manag.freeRef("bar");
    EXPECT_FALSE(manag.isFirstRef("foo"));
    EXPECT_TRUE(manag.hasNoRefs("foo"));
    EXPECT_FALSE(manag.isFirstRef("bar"));
    EXPECT_TRUE(manag.hasNoRefs("bar"));
}
