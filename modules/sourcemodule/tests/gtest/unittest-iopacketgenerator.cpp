#include <gtest/gtest.h>
#include "iogroupgenerator.h"

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

TEST(TEST_PACKET_GENERATIOR, SEND_NOT_EMPTY) {
    for(int type=SourceActionTypes::totalMinWithInvalid; type<=SourceActionTypes::totalMaxWithInvalid; ++type) {
        IoGroupGenerator ioGroupGenerator = IoGroupGenerator(QJsonObject());
        tIoTransferList outInList = ioGroupGenerator.generateListForAction(SourceActionTypes::ActionTypes(type));
        for(auto outIn : outInList) {
            EXPECT_FALSE(outIn->getBytesSend().isEmpty());
        }
    }
}

TEST(TEST_PACKET_GENERATIOR, TIMEOUT_SET) {
    IoGroupGenerator ioGroupGenerator = IoGroupGenerator(QJsonObject());
    JsonParamApi params;
    params.setOn(true);
    tSourceActionTypeList actionList = SourceActionGenerator::generateSwitchActions(params);
    IoQueueGroup::Ptr transferGroup = ioGroupGenerator.generateOnOffGroup(params);
    for(int idx=0; idx<transferGroup->getTransferCount(); idx++) {
        EXPECT_GE(transferGroup->getTransfer(idx)->getResponseTimeout(), 0);
    }
}

TEST(TEST_PACKET_GENERATIOR, EXPECT_RESPONSE_SET) {
    for(int type=SourceActionTypes::totalMinWithInvalid; type<=SourceActionTypes::totalMaxWithInvalid; ++type) {
        IoGroupGenerator ioGroupGenerator = IoGroupGenerator(QJsonObject());
        tIoTransferList outInList = ioGroupGenerator.generateListForAction(SourceActionTypes::ActionTypes(type));
        for(auto outIn : outInList) {
            QByteArray demoResponse = outIn->getDemoResponder()->getDemoResponse();
            EXPECT_GE(demoResponse.length(), 1);
            EXPECT_TRUE(demoResponse.endsWith("\r"));
        }
    }
}

TEST(TEST_PACKET_GENERATIOR, SWITCH_OFF_PACKET_SPECIFICS) {
    IoGroupGenerator ioGroupGenerator = IoGroupGenerator(QJsonObject());
    JsonParamApi params;
    params.setOn(false);
    IoQueueGroup::Ptr transferGroup = ioGroupGenerator.generateOnOffGroup(params);
    EXPECT_EQ(transferGroup->getErrorBehavior(), IoQueueErrorBehaviors::STOP_ON_ERROR);
}

TEST(TEST_PACKET_GENERATIOR, SWITCH_ON_PACKET_SPECIFICS) {
    IoGroupGenerator ioGroupGenerator = IoGroupGenerator(QJsonObject());
    JsonParamApi params;
    params.setOn(true);
    IoQueueGroup::Ptr transferGroup = ioGroupGenerator.generateOnOffGroup(params);
    EXPECT_EQ(transferGroup->getErrorBehavior(), IoQueueErrorBehaviors::STOP_ON_ERROR);
}

TEST(TEST_PACKET_GENERATIOR, POLL_PACKET_SPECIFICS) {
    IoGroupGenerator ioGroupGenerator = IoGroupGenerator(QJsonObject());
    IoQueueGroup::Ptr transferGroup = ioGroupGenerator.generateStatusPollGroup();
    EXPECT_EQ(transferGroup->getErrorBehavior(), IoQueueErrorBehaviors::CONTINUE_ON_ERROR);
}

