#include <gtest/gtest.h>
#include "sourceactions.h"

// SourceActionTypes::isValidType
TEST(TEST_SOURCEACTIONS, VALID_TYPE_DETECT) {
    for(int type=SourceActionTypes::totalMinWithInvalid-3; type<=SourceActionTypes::totalMaxWithInvalid+3; ++type) {
        //SourceActionTypes::ActionTypes forTest = SourceActionTypes::ActionTypes(type);
        bool isValid = SourceActionTypes::isValidType(SourceActionTypes::ActionTypes(type));
        if(isValid) {
            EXPECT_GT(type, SourceActionTypes::totalMinWithInvalid);

            EXPECT_NE(type, SourceActionTypes::SWITCH_UNDEF);
            EXPECT_NE(type, SourceActionTypes::SWITCH_UNDEF2);

            EXPECT_NE(type, SourceActionTypes::PERIODIC_UNDEF);
            EXPECT_NE(type, SourceActionTypes::PERIODIC_UNDEF2);

            EXPECT_LT(type, SourceActionTypes::totalMaxWithInvalid);
        }
    }
}

// Check all actions are generated for on
TEST(TEST_SOURCEACTIONS, SWITCH_ON_COMPLETE) {
    JsonParamApi paramApi;
    paramApi.setOn(true);
    tSourceActionTypeList actionList = SourceActionGenerator::generateSwitchActions(JsonParamApi(paramApi));
    EXPECT_EQ(actionList.count(), SourceActionTypes::switchTypeCount);
}

// Check for switch on all valid
TEST(TEST_SOURCEACTIONS, SWITCH_ON_VALID) {
    JsonParamApi paramApi;
    paramApi.setOn(true);
    tSourceActionTypeList actionList = SourceActionGenerator::generateSwitchActions(JsonParamApi(paramApi));
    for(auto action : actionList) {
        EXPECT_GT(action, SourceActionTypes::SWITCH_UNDEF);
        EXPECT_LT(action, SourceActionTypes::SWITCH_UNDEF2);
        EXPECT_TRUE(SourceActionTypes::isValidType(action));
    }
}

// Check for switch off just generates switch phases
TEST(TEST_SOURCEACTIONS, SWITCH_OFF_PHASE_ONLY) {
    JsonParamApi paramApi;
    paramApi.setOn(false);
    tSourceActionTypeList actionList = SourceActionGenerator::generateSwitchActions(paramApi);
    EXPECT_EQ(actionList.count(), 1);
    EXPECT_EQ(actionList[0], SourceActionTypes::ActionTypes::SWITCH_PHASES);
}

// Check for off are valid
TEST(TEST_SOURCEACTIONS, SWITCH_OFF_VALID) {
    JsonParamApi paramApi;
    paramApi.setOn(false);
    tSourceActionTypeList actionList = SourceActionGenerator::generateSwitchActions(JsonParamApi(paramApi));
    for(auto action : actionList) {
        EXPECT_GT(action, SourceActionTypes::SWITCH_UNDEF);
        EXPECT_LT(action, SourceActionTypes::SWITCH_UNDEF2);
        EXPECT_TRUE(SourceActionTypes::isValidType(action));
    }
}

TEST(TEST_SOURCEACTIONS, PERIODIC_VALID) {
    tSourceActionTypeList actionList = SourceActionGenerator::generatePeriodicActions();
    for(auto action : actionList) {
        EXPECT_GT(action, SourceActionTypes::PERIODIC_UNDEF);
        EXPECT_LT(action, SourceActionTypes::PERIODIC_UNDEF2);
        EXPECT_TRUE(SourceActionTypes::isValidType(action));
    }
}

TEST(TEST_SOURCEACTIONS, PERIODIC_COMPLETE) {
    tSourceActionTypeList actionList = SourceActionGenerator::generatePeriodicActions();
    EXPECT_EQ(actionList.count(), SourceActionTypes::periodicTypeCount);
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
    EXPECT_EQ(actionListSorted.count(), entryCount);
    EXPECT_EQ(actionListSorted[0], 3);
    EXPECT_EQ(actionListSorted[1], 4);
    EXPECT_EQ(actionListSorted[2], 1);
    EXPECT_EQ(actionListSorted[3], 2);
}

TEST(TEST_SOURCEACTIONS, SORT_TO_BACK) {
    tTestList actionList;
    actionList.append(1);
    actionList.append(2);
    actionList.append(3);
    actionList.append(4);
    int entryCount = actionList.count();

    tTestList actionListSorted = sortListCustom(actionList, tTestList() << 1 << 2, false);
    EXPECT_EQ(actionListSorted.count(), entryCount);
    EXPECT_EQ(actionListSorted[0], 3);
    EXPECT_EQ(actionListSorted[1], 4);
    EXPECT_EQ(actionListSorted[2], 1);
    EXPECT_EQ(actionListSorted[3], 2);
}

TEST(TEST_SOURCEACTIONS, SORT_KEEP_SEQUENCE) {
    tTestList actionList;
    actionList.append(1);
    actionList.append(2);
    actionList.append(3);
    actionList.append(4);
    int entryCount = actionList.count();

    tTestList actionListSorted = sortListCustom(actionList, tTestList(), true);
    EXPECT_EQ(actionListSorted.count(), entryCount);
    EXPECT_EQ(actionListSorted[0], 1);
    EXPECT_EQ(actionListSorted[1], 2);
    EXPECT_EQ(actionListSorted[2], 3);
    EXPECT_EQ(actionListSorted[3], 4);
}
