#include <gtest/gtest.h>
#include "sourceidgenerator.h"

TEST(TEST_TRANSACTION_ID_GENERATOR, NOT_ZERO) {
    cSourceIdGenerator generator;
    int id;
    id = generator.nextID();
    EXPECT_EQ(id, 1);
    id = generator.nextID();
    EXPECT_EQ(id, 2);
    id = generator.nextID();
    EXPECT_EQ(id, 3);
}
