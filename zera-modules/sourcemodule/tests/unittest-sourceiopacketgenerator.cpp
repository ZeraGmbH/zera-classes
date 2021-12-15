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

TEST(TEST_PACKET_GENERATIOR, EMPTY_IO_UNDEFINED) {
    cSourceSingleOutIn outIn;
    EXPECT_EQ(outIn.m_actionType, cSourceActionTypes::SWITCH_UNDEF);
    EXPECT_EQ(outIn.m_responseType, RESP_UNDEFINED);
    EXPECT_EQ(outIn.m_responseTimeoutMs, 0);
}

TEST(TEST_PACKET_GENERATIOR, TYPE_SET) {
    for(int type=cSourceActionTypes::totalMinWithInvalid; type<=cSourceActionTypes::totalMaxWithInvalid; ++type) {
        cSourceIoPacketGenerator ioPackGenerator = cSourceIoPacketGenerator(QJsonObject());
        tSourceOutInList outInList = ioPackGenerator.generateListForAction(cSourceActionTypes::ActionTypes(type));
        for(auto outIn : outInList) {
            EXPECT_EQ(outIn.m_actionType, type);
        }
    }
}

TEST(TEST_PACKET_GENERATIOR, SEND_NOT_EMPTY) {
    for(int type=cSourceActionTypes::totalMinWithInvalid; type<=cSourceActionTypes::totalMaxWithInvalid; ++type) {
        cSourceIoPacketGenerator ioPackGenerator = cSourceIoPacketGenerator(QJsonObject());
        tSourceOutInList outInList = ioPackGenerator.generateListForAction(cSourceActionTypes::ActionTypes(type));
        for(auto outIn : outInList) {
            EXPECT_FALSE(outIn.m_bytesSend.isEmpty());
        }
    }
}

TEST(TEST_PACKET_GENERATIOR, RESPONSE_TYPE_SET) {
    for(int type=cSourceActionTypes::totalMinWithInvalid; type<=cSourceActionTypes::totalMaxWithInvalid; ++type) {
        cSourceIoPacketGenerator ioPackGenerator = cSourceIoPacketGenerator(QJsonObject());
        tSourceOutInList outInList = ioPackGenerator.generateListForAction(cSourceActionTypes::ActionTypes(type));
        for(auto outIn : outInList) {
            EXPECT_FALSE(outIn.m_responseType == RESP_UNDEFINED);
            EXPECT_FALSE(outIn.m_responseType >= RESP_UNDEF_BOTTOM);
        }
    }
}

TEST(TEST_PACKET_GENERATIOR, TIMEOUT_SET) {
    cSourceIoPacketGenerator ioPackGenerator = cSourceIoPacketGenerator(QJsonObject());
    SourceJsonParamApi params;
    params.setOn(true);
    tSourceActionTypeList actionList = cSourceActionGenerator::generateSwitchActions(params);
    cSourceCommandPacket cmdPack = ioPackGenerator.generateOnOffPacket(params);
    for(auto outIn : cmdPack.m_outInList) {
        EXPECT_FALSE(outIn.m_responseTimeoutMs == 0);
    }
}

TEST(TEST_PACKET_GENERATIOR, EXPECT_RESPONSE_SET) {
    for(int type=cSourceActionTypes::totalMinWithInvalid; type<=cSourceActionTypes::totalMaxWithInvalid; ++type) {
        cSourceIoPacketGenerator ioPackGenerator = cSourceIoPacketGenerator(QJsonObject());
        tSourceOutInList outInList = ioPackGenerator.generateListForAction(cSourceActionTypes::ActionTypes(type));
        for(auto outIn : outInList) {
            if(outIn.m_responseType == RESP_FULL_DATA_SEQUENCE || outIn.m_responseType == RESP_PART_DATA_SEQUENCE) {
                EXPECT_FALSE(outIn.m_bytesExpected.isEmpty());
            }
        }
    }
}

TEST(TEST_PACKET_GENERATIOR, PACK_GENERATIR_NOT_INVENTING) {
    cSourceIoPacketGenerator ioPackGenerator = cSourceIoPacketGenerator(QJsonObject());
    SourceJsonParamApi params;
    params.setOn(true);
    tSourceActionTypeList actionList = cSourceActionGenerator::generateSwitchActions(params);
    cSourceCommandPacket cmdPack = ioPackGenerator.generateOnOffPacket(params);
    for(auto outIn : cmdPack.m_outInList) {
        auto type = outIn.m_actionType;
        EXPECT_TRUE(actionList.contains(type));
    }
}

TEST(TEST_PACKET_GENERATIOR, SWITCH_PACKET_SPECIFICS) {
    cSourceIoPacketGenerator ioPackGenerator = cSourceIoPacketGenerator(QJsonObject());
    SourceJsonParamApi params;
    params.setOn(false);
    cSourceCommandPacket cmdPack = ioPackGenerator.generateOnOffPacket(params);
    EXPECT_EQ(cmdPack.m_commandType, COMMAND_SWITCH);
    EXPECT_EQ(cmdPack.m_errorBehavior, BEHAVE_STOP_ON_ERROR);
}

TEST(TEST_PACKET_GENERATIOR, POLL_PACKET_SPECIFICS) {
    cSourceIoPacketGenerator ioPackGenerator = cSourceIoPacketGenerator(QJsonObject());
    cSourceCommandPacket cmdPack = ioPackGenerator.generateStatusPollPacket();
    EXPECT_EQ(cmdPack.m_commandType, COMMAND_STATE_POLL);
    EXPECT_EQ(cmdPack.m_errorBehavior, BEHAVE_CONTINUE_ON_ERROR);
}

