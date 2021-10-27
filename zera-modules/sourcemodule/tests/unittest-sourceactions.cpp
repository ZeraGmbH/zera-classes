#include <gtest/gtest.h>
#include "sourceactions.h"

// Check all actions are generated
TEST(TEST_SOURCEACTIONS, ACTIONS_COMPLETE) {
    tSourceActionMap actionMap = cSourceActionGenerator::GenerateLoadActionMap(QJsonObject(), QJsonObject());
    EXPECT_EQ(actionMap.count(), cSourceActionTypes::getLoadActionTypeCount());
}

TEST(TEST_SOURCEACTIONS, PERIODIC_ACTIONS_COMPLETE) {
    tSourceActionMap periodicActionMap = cSourceActionGenerator::GeneratePeriodicActionMap(QJsonObject());
    EXPECT_EQ(periodicActionMap.count(), cSourceActionTypes::getPeriodicActionTypeCount());
}
