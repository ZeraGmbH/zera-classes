#include <gtest/gtest.h>
#include "sourceactions.h"
#include "sourcejsonapi.h"

// Check all actions are generated for on
TEST(TEST_SOURCEACTIONS, ACTIONS_ON_COMPLETE) {
    cSourceJsonParamApi paramApi;
    paramApi.setOn(true);
    tSourceActionTypeList actionList = cSourceActionGenerator::generateLoadActions(cSourceJsonParamApi(paramApi));
    EXPECT_EQ(actionList.count(), cSourceActionTypes::getLoadActionTypeCount());
}

// Check for on are valid
TEST(TEST_SOURCEACTIONS, ACTIONS_ON_VALID) {
    cSourceJsonParamApi paramApi;
    paramApi.setOn(true);
    tSourceActionTypeList actionList = cSourceActionGenerator::generateLoadActions(cSourceJsonParamApi(paramApi));
    for(auto action : actionList) {
        EXPECT_FALSE(action == cSourceActionTypes::ACTION_UNDEF);
        EXPECT_FALSE(action == cSourceActionTypes::LOAD_ACTION_COUNT);
        EXPECT_FALSE(action == cSourceActionTypes::PERIODIC_LAST);
    }
}

// Check if a switch off just generates switch phases
TEST(TEST_SOURCEACTIONS, ACTIONS_OFF_PHASE_ONLY) {
    cSourceJsonParamApi paramApi;
    paramApi.setOn(false);
    tSourceActionTypeList actionList = cSourceActionGenerator::generateLoadActions(paramApi);
    EXPECT_EQ(actionList.count(), 1);
    EXPECT_EQ(actionList.contains(cSourceActionTypes::ActionTypes::SWITCH_PHASES), true);
}

// Check for off are valid
TEST(TEST_SOURCEACTIONS, ACTIONS_OFF_VALID) {
    cSourceJsonParamApi paramApi;
    paramApi.setOn(false);
    tSourceActionTypeList actionList = cSourceActionGenerator::generateLoadActions(cSourceJsonParamApi(paramApi));
    for(auto action : actionList) {
        EXPECT_FALSE(action == cSourceActionTypes::ACTION_UNDEF);
        EXPECT_FALSE(action == cSourceActionTypes::LOAD_ACTION_COUNT);
        EXPECT_FALSE(action == cSourceActionTypes::PERIODIC_LAST);
    }
}

TEST(TEST_SOURCEACTIONS, PERIODIC_ACTIONS_COMPLETE) {
    tSourceActionTypeList periodicActionList = cSourceActionGenerator::generatePeriodicActions();
    EXPECT_EQ(periodicActionList.count(), cSourceActionTypes::getPeriodicActionTypeCount());
}

typedef QList<int> tTestList;

TEST(TEST_SOURCEACTIONS, SORT_KEEP_SIZE) {
    tTestList actionList;
    actionList.append(1);
    actionList.append(2);
    actionList.append(3);
    actionList.append(4);

    tTestList actionListSorted = sortListCustom(actionList, tTestList(), true);
    EXPECT_EQ(actionList.count(), actionListSorted.count());
}

TEST(TEST_SOURCEACTIONS, SORT_TO_FRONT) {
    tTestList actionList;
    actionList.append(1);
    actionList.append(2);
    actionList.append(3);
    actionList.append(4);
    int entryCount = actionList.count();

    tTestList actionListSorted = sortListCustom(actionList, tTestList() << 3 << 4, true);
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

    tTestList actionListSorted = sortListCustom(actionList, tTestList() << 1 << 2, false);
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

    tTestList actionListSorted = sortListCustom(actionList, tTestList(), true);
    EXPECT_EQ(actionListSorted.count() == entryCount && actionListSorted[0] == 1, true);
    EXPECT_EQ(actionListSorted.count() == entryCount && actionListSorted[1] == 2, true);
    EXPECT_EQ(actionListSorted.count() == entryCount && actionListSorted[2] == 3, true);
    EXPECT_EQ(actionListSorted.count() == entryCount && actionListSorted[3] == 4, true);
}
