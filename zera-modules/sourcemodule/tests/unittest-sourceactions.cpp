#include <gtest/gtest.h>
#include "sourceactions.h"

// Check all actions are generated
TEST(TEST_SOURCEACTIONS, ACTIONS_COMPLETE) {
    tSourceActionTypeList actionMap = cSourceActionGenerator::GenerateLoadActionList(QJsonObject());
    EXPECT_EQ(actionMap.count(), cSourceActionTypes::getLoadActionTypeCount());
}

// Check if a switch off just generates switch phases
// We do this test because we want to be warned in case we change behaviour
TEST(TEST_SOURCEACTIONS, ACTIONS_OFF_PHASE_ONLY) {
    QJsonObject offParamState;
    offParamState.insert("on", false);
    tSourceActionTypeList actionMap = cSourceActionGenerator::GenerateLoadActionList(offParamState);
    EXPECT_EQ(actionMap.count(), 1);
    EXPECT_EQ(actionMap.contains(cSourceActionTypes::ActionTypes::SWITCH_PHASES), true);
}

TEST(TEST_SOURCEACTIONS, PERIODIC_ACTIONS_COMPLETE) {
    tSourceActionTypeList periodicActionMap = cSourceActionGenerator::GeneratePeriodicActionList();
    EXPECT_EQ(periodicActionMap.count(), cSourceActionTypes::getPeriodicActionTypeCount());
}

typedef QList<int> tTestList;

TEST(TEST_SOURCEACTIONS, SORT_KEEP_SIZE) {
    tTestList actionList;
    actionList.append(1);
    actionList.append(2);
    actionList.append(3);
    actionList.append(4);

    tTestList actionListSorted = listSortCustom(actionList, tTestList(), true);
    EXPECT_EQ(actionList.count(), actionListSorted.count());
}

TEST(TEST_SOURCEACTIONS, SORT_TO_FRONT) {
    tTestList actionList;
    actionList.append(1);
    actionList.append(2);
    actionList.append(3);
    actionList.append(4);
    int entryCount = actionList.count();

    tTestList actionListSorted = listSortCustom(actionList, tTestList() << 3 << 4, true);
    EXPECT_EQ(actionListSorted.count() == entryCount && actionListSorted[0] == 3, true);
    EXPECT_EQ(actionListSorted.count() == entryCount && actionListSorted[1] == 4, true);
    EXPECT_EQ(actionListSorted.count() == entryCount && actionListSorted[2] == 1, true);
    EXPECT_EQ(actionListSorted.count() == entryCount && actionListSorted[3] == 2, true);
}

TEST(TEST_SOURCEACTIONS, SORT_TO_BACK) {
    tTestList actionList;
    actionList.append(1);
    actionList.append(2);
    actionList.append(3);
    actionList.append(4);
    int entryCount = actionList.count();

    tTestList actionListSorted = listSortCustom(actionList, tTestList() << 1 << 2, false);
    EXPECT_EQ(actionListSorted.count() == entryCount && actionListSorted[0] == 3, true);
    EXPECT_EQ(actionListSorted.count() == entryCount && actionListSorted[1] == 4, true);
    EXPECT_EQ(actionListSorted.count() == entryCount && actionListSorted[2] == 1, true);
    EXPECT_EQ(actionListSorted.count() == entryCount && actionListSorted[3] == 2, true);
}

TEST(TEST_SOURCEACTIONS, SORT_KEEP_SEQUENCE) {
    tTestList actionList;
    actionList.append(1);
    actionList.append(2);
    actionList.append(3);
    actionList.append(4);
    int entryCount = actionList.count();

    tTestList actionListSorted = listSortCustom(actionList, tTestList(), true);
    EXPECT_EQ(actionListSorted.count() == entryCount && actionListSorted[0] == 1, true);
    EXPECT_EQ(actionListSorted.count() == entryCount && actionListSorted[1] == 2, true);
    EXPECT_EQ(actionListSorted.count() == entryCount && actionListSorted[2] == 3, true);
    EXPECT_EQ(actionListSorted.count() == entryCount && actionListSorted[3] == 4, true);
}
