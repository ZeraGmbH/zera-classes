#include <gtest/gtest.h>
#include "idgenerator.h"

TEST(TEST_ID_KEEPER_SINGLE, INCREMENT) {
    IoIdGenerator generator;
    int id1  = generator.nextID();
    int id2 = generator.nextID();
    int id3 = generator.nextID();
    EXPECT_NE(id1, id2);
    EXPECT_NE(id2, id3);
    EXPECT_NE(id1, id3);
}
