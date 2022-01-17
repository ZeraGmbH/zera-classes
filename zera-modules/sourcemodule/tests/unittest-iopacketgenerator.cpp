#include <gtest/gtest.h>
#include "io-worker/iopacketgenerator.h"

// double -> send string conversion
TEST(TEST_PACKET_GENERATIOR, VALUE_CONVERSION) {
    QByteArray valGenerated = IoCmdFormatHelper::formatDouble(5.1, 3, '.', 3);
    QByteArray valExpected = "005.100";
    EXPECT_EQ(valGenerated, valExpected);

    valGenerated = IoCmdFormatHelper::formatDouble(10.2, 3, '.', 2);
    valExpected = "010.20";
    EXPECT_EQ(valGenerated, valExpected);

    valGenerated = IoCmdFormatHelper::formatDouble(50.02, 2, '.', 2);
    valExpected = "50.02";
    EXPECT_EQ(valGenerated, valExpected);

    valGenerated = IoCmdFormatHelper::formatDouble(0.0, 2, '.', 2);
    valExpected = "00.00";
    EXPECT_EQ(valGenerated, valExpected);
}

TEST(TEST_PACKET_GENERATIOR, EMPTY_IO_UNDEFINED) {
    IoSingleOutIn outIn;
    EXPECT_EQ(outIn.m_actionType, SourceActionTypes::SWITCH_UNDEF);
    EXPECT_EQ(outIn.m_responseTimeoutMs, 0);
}

TEST(TEST_PACKET_GENERATIOR, TYPE_SET) {
    for(int type=SourceActionTypes::totalMinWithInvalid; type<=SourceActionTypes::totalMaxWithInvalid; ++type) {
        IoPacketGenerator ioPackGenerator = IoPacketGenerator(QJsonObject());
        tIoOutInList outInList = ioPackGenerator.generateListForAction(SourceActionTypes::ActionTypes(type));
        for(auto outIn : outInList) {
            EXPECT_EQ(outIn.m_actionType, type);
        }
    }
}

TEST(TEST_PACKET_GENERATIOR, SEND_NOT_EMPTY) {
    for(int type=SourceActionTypes::totalMinWithInvalid; type<=SourceActionTypes::totalMaxWithInvalid; ++type) {
        IoPacketGenerator ioPackGenerator = IoPacketGenerator(QJsonObject());
        tIoOutInList outInList = ioPackGenerator.generateListForAction(SourceActionTypes::ActionTypes(type));
        for(auto outIn : outInList) {
            EXPECT_FALSE(outIn.m_bytesSend.isEmpty());
        }
    }
}

TEST(TEST_PACKET_GENERATIOR, TIMEOUT_SET) {
    IoPacketGenerator ioPackGenerator = IoPacketGenerator(QJsonObject());
    JsonParamApi params;
    params.setOn(true);
    tSourceActionTypeList actionList = SourceActionGenerator::generateSwitchActions(params);
    IoCommandPacket cmdPack = ioPackGenerator.generateOnOffPacket(params);
    for(auto outIn : cmdPack.m_outInList) {
        EXPECT_FALSE(outIn.m_responseTimeoutMs == 0);
    }
}

TEST(TEST_PACKET_GENERATIOR, EXPECT_RESPONSE_SET) {
    for(int type=SourceActionTypes::totalMinWithInvalid; type<=SourceActionTypes::totalMaxWithInvalid; ++type) {
        IoPacketGenerator ioPackGenerator = IoPacketGenerator(QJsonObject());
        tIoOutInList outInList = ioPackGenerator.generateListForAction(SourceActionTypes::ActionTypes(type));
        for(auto outIn : outInList) {
            EXPECT_FALSE(outIn.m_bytesExpected.isEmpty());
        }
    }
}

TEST(TEST_PACKET_GENERATIOR, PACK_GENERATIR_NOT_INVENTING) {
    IoPacketGenerator ioPackGenerator = IoPacketGenerator(QJsonObject());
    JsonParamApi params;
    params.setOn(true);
    tSourceActionTypeList actionList = SourceActionGenerator::generateSwitchActions(params);
    IoCommandPacket cmdPack = ioPackGenerator.generateOnOffPacket(params);
    for(auto outIn : cmdPack.m_outInList) {
        auto type = outIn.m_actionType;
        EXPECT_TRUE(actionList.contains(type));
    }
}

TEST(TEST_PACKET_GENERATIOR, SWITCH_OFF_PACKET_SPECIFICS) {
    IoPacketGenerator ioPackGenerator = IoPacketGenerator(QJsonObject());
    JsonParamApi params;
    params.setOn(false);
    IoCommandPacket cmdPack = ioPackGenerator.generateOnOffPacket(params);
    EXPECT_EQ(cmdPack.m_commandType, COMMAND_SWITCH_OFF);
    EXPECT_EQ(cmdPack.m_errorBehavior, BEHAVE_STOP_ON_ERROR);
}

TEST(TEST_PACKET_GENERATIOR, SWITCH_ON_PACKET_SPECIFICS) {
    IoPacketGenerator ioPackGenerator = IoPacketGenerator(QJsonObject());
    JsonParamApi params;
    params.setOn(true);
    IoCommandPacket cmdPack = ioPackGenerator.generateOnOffPacket(params);
    EXPECT_EQ(cmdPack.m_commandType, COMMAND_SWITCH_ON);
    EXPECT_EQ(cmdPack.m_errorBehavior, BEHAVE_STOP_ON_ERROR);
}

TEST(TEST_PACKET_GENERATIOR, POLL_PACKET_SPECIFICS) {
    IoPacketGenerator ioPackGenerator = IoPacketGenerator(QJsonObject());
    IoCommandPacket cmdPack = ioPackGenerator.generateStatusPollPacket();
    EXPECT_EQ(cmdPack.m_commandType, COMMAND_STATE_POLL);
    EXPECT_EQ(cmdPack.m_errorBehavior, BEHAVE_CONTINUE_ON_ERROR);
}

