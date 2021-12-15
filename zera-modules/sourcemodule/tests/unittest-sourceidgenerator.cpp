#include <gtest/gtest.h>
#include "sourceidgenerator.h"

TEST(TEST_ID_GENERATOR, INCREMENT) {
    SourceIdGenerator generator;
    int id;
    id = generator.nextID();
    EXPECT_EQ(id, 0);
    id = generator.nextID();
    EXPECT_EQ(id, 1);
    id = generator.nextID();
    EXPECT_EQ(id, 2);
}
