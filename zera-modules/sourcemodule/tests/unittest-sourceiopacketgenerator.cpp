#include <gtest/gtest.h>
#include "sourceiopacketgenerator.h"

// double -> send string conversion
TEST(TEST_PACKET_GENERATIOR, VALUE_CONVERSION) {
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

TEST(TEST_PACKET_GENERATIOR, TYPE_SET) {
    for(int type=cSourceActionTypes::totalMinWithInvalid; type<=cSourceActionTypes::totalMaxWithInvalid; ++type) {
        cSourceIoPacketGenerator transactionGenerator = cSourceIoPacketGenerator(QJsonObject());
        tSourceOutInList transactionList = transactionGenerator.generateListForAction(cSourceActionTypes::ActionTypes(type));
        for(auto transaction : transactionList) {
            EXPECT_EQ(transaction.m_actionType, type);
        }
    }
}

TEST(TEST_PACKET_GENERATIOR, SEND_NOT_EMPTY) {
    for(int type=cSourceActionTypes::totalMinWithInvalid; type<=cSourceActionTypes::totalMaxWithInvalid; ++type) {
        cSourceIoPacketGenerator transactionGenerator = cSourceIoPacketGenerator(QJsonObject());
        tSourceOutInList transactionList = transactionGenerator.generateListForAction(cSourceActionTypes::ActionTypes(type));
        for(auto transaction : transactionList) {
            EXPECT_FALSE(transaction.m_bytesSend.isEmpty());
        }
    }
}

TEST(TEST_PACKET_GENERATIOR, RESPONSE_TYPE_SET) {
    for(int type=cSourceActionTypes::totalMinWithInvalid; type<=cSourceActionTypes::totalMaxWithInvalid; ++type) {
        cSourceIoPacketGenerator transactionGenerator = cSourceIoPacketGenerator(QJsonObject());
        tSourceOutInList transactionList = transactionGenerator.generateListForAction(cSourceActionTypes::ActionTypes(type));
        for(auto transaction : transactionList) {
            EXPECT_FALSE(transaction.m_responseType == RESP_UNDEFINED);
            EXPECT_FALSE(transaction.m_responseType >= RESP_UNDEF_BOTTOM);
        }
    }
}

TEST(TEST_PACKET_GENERATIOR, EXPECT_RESPONSE_SET) {
    for(int type=cSourceActionTypes::totalMinWithInvalid; type<=cSourceActionTypes::totalMaxWithInvalid; ++type) {
        cSourceIoPacketGenerator transactionGenerator = cSourceIoPacketGenerator(QJsonObject());
        tSourceOutInList transactionList = transactionGenerator.generateListForAction(cSourceActionTypes::ActionTypes(type));
        for(auto transaction : transactionList) {
            if(transaction.m_responseType == RESP_FULL_DATA_SEQUENCE || transaction.m_responseType == RESP_PART_DATA_SEQUENCE) {
                EXPECT_FALSE(transaction.m_bytesExpected.isEmpty());
            }
        }
    }
}

TEST(TEST_PACKET_GENERATIOR, SWITCH_PACKET_SPECIFICS) {
    cSourceIoPacketGenerator transactionGenerator = cSourceIoPacketGenerator(QJsonObject());
    cSourceJsonParamApi params;
    params.setOn(false);
    cSourceCommandPacket cmpPack = transactionGenerator.generateOnOffPacket(params);
    EXPECT_EQ(cmpPack.m_commandType, COMMAND_SWITCH);
    EXPECT_EQ(cmpPack.m_errorBehavior, BEHAVE_STOP_ON_ERROR);
}

TEST(TEST_PACKET_GENERATIOR, POLL_PACKET_SPECIFICS) {
    cSourceIoPacketGenerator transactionGenerator = cSourceIoPacketGenerator(QJsonObject());
    cSourceCommandPacket cmpPack = transactionGenerator.generateStatusPollPacket();
    EXPECT_EQ(cmpPack.m_commandType, COMMAND_STATE_POLL);
    EXPECT_EQ(cmpPack.m_errorBehavior, BEHAVE_CONTINUE_ON_ERROR);
}

