#include <gtest/gtest.h>
#include "io-ids/ioidgenerator.h"

TEST(TEST_ID_GENERATOR, INCREMENT) {
    IoIdGenerator generator;
    int id1  = generator.nextID();
    int id2 = generator.nextID();
    int id3 = generator.nextID();
    EXPECT_NE(id1, id2);
    EXPECT_NE(id2, id3);
    EXPECT_NE(id1, id3);
}

TEST(TEST_ID_GENERATOR, INCREMENT_STATIC) {
    IoIdGenerator generator;
    int id1  = generator.nextIDStatic();
    int id2 = generator.nextIDStatic();
    int id3 = generator.nextIDStatic();
    EXPECT_NE(id1, id2);
    EXPECT_NE(id2, id3);
    EXPECT_NE(id1, id3);
}
