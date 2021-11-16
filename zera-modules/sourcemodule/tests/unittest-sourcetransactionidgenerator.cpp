#include <gtest/gtest.h>
#include "sourcetransactionidgenerator.h"

TEST(TEST_TRANSACTION_ID_GENERATOR, NOT_ZERO) {
    cSourceTransactionIdGenerator generator;
    int id;
    id = generator.nextTransactionID();
    EXPECT_EQ(id, 1);
    id = generator.nextTransactionID();
    EXPECT_EQ(id, 2);
    id = generator.nextTransactionID();
    EXPECT_EQ(id, 3);
}
