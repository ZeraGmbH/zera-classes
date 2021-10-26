#include <gtest/gtest.h>
#include "sourceactions.h"

// Check all actions are generated
TEST(TEST_SOURCEACTIONS, ACTIONS_COMPLETE) {
    tActionMap actionMap = cSourceActionGenerator::GenerateActionMap(QJsonObject(), QJsonObject());
    EXPECT_EQ(actionMap.count(), cSourceAction::ACTION_COUNT);
}

TEST(TEST_SOURCEACTIONS, PERIODIC_ACTIONS_COMPLETE) {
    tPeriodicActionMap periodicActionMap = cSourceActionGenerator::GeneratePeriodicActionMap(QJsonObject(), QJsonObject());
    EXPECT_EQ(periodicActionMap.count(), cSourceAction::PERIODIC_ACTION_COUNT);
}
