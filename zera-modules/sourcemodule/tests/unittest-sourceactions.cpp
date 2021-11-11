#include <gtest/gtest.h>
#include "sourceactions.h"

// Check all actions are generated
TEST(TEST_SOURCEACTIONS, ACTIONS_COMPLETE) {
    tSourceActionMap actionMap = cSourceActionGenerator::GenerateLoadActionMap(QJsonObject(), QJsonObject());
    EXPECT_EQ(actionMap.count(), cSourceActionTypes::getLoadActionTypeCount());
}

// Check if a switch off just generates switch phases
// We do this test because we want to be warned in case we change behaviour
TEST(TEST_SOURCEACTIONS, ACTIONS_OFF_PHASE_ONLY) {
    QJsonObject offParamState;
    offParamState.insert("on", false);
    tSourceActionMap actionMap = cSourceActionGenerator::GenerateLoadActionMap(QJsonObject(), offParamState);
    EXPECT_EQ(actionMap.count(), 1);
    EXPECT_EQ(actionMap.contains(cSourceActionTypes::ActionTypes::SWITCH_PHASES), true);
}

TEST(TEST_SOURCEACTIONS, PERIODIC_ACTIONS_COMPLETE) {
    tSourceActionMap periodicActionMap = cSourceActionGenerator::GeneratePeriodicActionMap(QJsonObject());
    EXPECT_EQ(periodicActionMap.count(), cSourceActionTypes::getPeriodicActionTypeCount());
}

TEST(TEST_SOURCEACTIONS, SORT_KEEP_SIZE) {
    tSourceActionMap actionMap;
    cSourceAction dummyAction;
    // no cSourceActionGenerator dependency -> create manually
    actionMap[cSourceActionTypes::SET_RMS] = dummyAction;
    actionMap[cSourceActionTypes::SET_ANGLE] = dummyAction;
    actionMap[cSourceActionTypes::SET_FREQUENCY] = dummyAction;
    tSourceActionList actionList = cSourceActionSorter::SortActionMap(actionMap, QList<cSourceActionTypes::ActionTypes>());
    EXPECT_EQ(actionMap.count(), actionList.count());
}

TEST(TEST_SOURCEACTIONS, SORT_TO_FRONT) {
    tSourceActionMap actionMap;
    cSourceAction dummyAction;
    actionMap[cSourceActionTypes::SET_RMS] = dummyAction;
    actionMap[cSourceActionTypes::SET_ANGLE] = dummyAction;
    actionMap[cSourceActionTypes::SET_FREQUENCY] = dummyAction;
    tSourceActionList actionList = cSourceActionSorter::SortActionMap(
                actionMap, QList<cSourceActionTypes::ActionTypes>() << cSourceActionTypes::SET_FREQUENCY);
    EXPECT_EQ(actionList.count() == 3 && actionList[0].actionType == cSourceActionTypes::SET_FREQUENCY, true);
    EXPECT_EQ(actionList.count() == 3 && actionList[1].actionType == cSourceActionTypes::SET_RMS, true);
    EXPECT_EQ(actionList.count() == 3 && actionList[2].actionType == cSourceActionTypes::SET_ANGLE, true);
}

TEST(TEST_SOURCEACTIONS, SORT_KEEP_SEQUENCE) {
    cSourceAction dummyAction;
    tSourceActionMap actionMap;
    actionMap[cSourceActionTypes::SET_RMS] = dummyAction;
    actionMap[cSourceActionTypes::SET_ANGLE] = dummyAction;
    actionMap[cSourceActionTypes::SET_FREQUENCY] = dummyAction;
    tSourceActionList actionList = cSourceActionSorter::SortActionMap(
                actionMap, QList<cSourceActionTypes::ActionTypes>());
    EXPECT_EQ(actionList.count() == 3 && actionList[0].actionType == cSourceActionTypes::SET_RMS, true);
    EXPECT_EQ(actionList.count() == 3 && actionList[1].actionType == cSourceActionTypes::SET_ANGLE, true);
    EXPECT_EQ(actionList.count() == 3 && actionList[2].actionType == cSourceActionTypes::SET_FREQUENCY, true);
}
