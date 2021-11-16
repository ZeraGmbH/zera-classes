#include <gtest/gtest.h>
#include "sourceiopacketgenerator.h"

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

TEST(TEST_SOURCE_IO_TRANSACTION, TRANSACTION_TYPE_SET) {
    for(int action = 0; action<cSourceActionTypes::ACTION_LIMIT; action++) {
        cSourceIoPacketGenerator transactionGenerator = cSourceIoPacketGenerator(QJsonObject());
        tSourceOutInList transactionList = transactionGenerator.generateListForAction(cSourceActionTypes::ActionTypes(action));
        for(auto transaction : transactionList) {
            EXPECT_EQ(transaction.m_actionType, action);
        }
    }
}

TEST(TEST_SOURCE_IO_TRANSACTION, TRANSACTION_SEND_NOT_EMPTY) {
    for(int action = 0; action<cSourceActionTypes::ACTION_LIMIT; action++) {
        cSourceIoPacketGenerator transactionGenerator = cSourceIoPacketGenerator(QJsonObject());
        tSourceOutInList transactionList = transactionGenerator.generateListForAction(cSourceActionTypes::ActionTypes(action));
        for(auto transaction : transactionList) {
            EXPECT_FALSE(transaction.m_bytesSend.isEmpty());
        }
    }
}

TEST(TEST_SOURCE_IO_TRANSACTION, TRANSACTION_RESPONSE_TYPE_SET) {
    for(int action = 0; action<cSourceActionTypes::ACTION_LIMIT; action++) {
        cSourceIoPacketGenerator transactionGenerator = cSourceIoPacketGenerator(QJsonObject());
        tSourceOutInList transactionList = transactionGenerator.generateListForAction(cSourceActionTypes::ActionTypes(action));
        for(auto transaction : transactionList) {
            EXPECT_FALSE(transaction.m_responseType == RESP_UNDEFINED);
            EXPECT_FALSE(transaction.m_responseType >= RESP_LIMIT);
        }
    }
}

TEST(TEST_SOURCE_IO_TRANSACTION, TRANSACTION_EXPECT_RESPONSE_SET) {
    for(int action = 0; action<cSourceActionTypes::ACTION_LIMIT; action++) {
        cSourceIoPacketGenerator transactionGenerator = cSourceIoPacketGenerator(QJsonObject());
        tSourceOutInList transactionList = transactionGenerator.generateListForAction(cSourceActionTypes::ActionTypes(action));
        for(auto transaction : transactionList) {
            if(transaction.m_responseType == RESP_FULL_DATA_SEQUENCE || transaction.m_responseType == RESP_PART_DATA_SEQUENCE) {
                EXPECT_FALSE(transaction.m_bytesExpected.isEmpty());
            }
        }
    }
}
