#include <gtest/gtest.h>
#include "sourceiotransactiongenerator.h"

// double -> send string conversion
TEST(TEST_SOURCE_IO_TRANSACTION, VALUE_CONVERSION) {
    QByteArray valGenerated = cSourceIoCmdHelper::formatDouble(5.1, 3, '.', 3);
    QByteArray valExpected = "005.100";
    EXPECT_EQ(valGenerated, valExpected);

    valGenerated = cSourceIoCmdHelper::formatDouble(10.2, 3, '.', 2);
    valExpected = "010.20";
    EXPECT_EQ(valGenerated, valExpected);

    valGenerated = cSourceIoCmdHelper::formatDouble(50.02, 2, '.', 2);
    valExpected = "50.02";
    EXPECT_EQ(valGenerated, valExpected);

    valGenerated = cSourceIoCmdHelper::formatDouble(0.0, 2, '.', 2);
    valExpected = "00.00";
    EXPECT_EQ(valGenerated, valExpected);
}

