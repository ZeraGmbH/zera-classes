#include <gtest/gtest.h>
#include "sourceactions.h"
#include "sourcejsonapi.h"

// cSourceActionTypes::isValidType
TEST(TEST_SOURCEACTIONS, VALID_TYPE_DETECT) {
    for(int type=cSourceActionTypes::totalMinWithInvalid-3; type<=cSourceActionTypes::totalMaxWithInvalid+3; ++type) {
        //cSourceActionTypes::ActionTypes forTest = cSourceActionTypes::ActionTypes(type);
        bool isValid = cSourceActionTypes::isValidType(cSourceActionTypes::ActionTypes(type));
        if(isValid) {
            EXPECT_GT(type, cSourceActionTypes::totalMinWithInvalid);

            EXPECT_NE(type, cSourceActionTypes::SWITCH_UNDEF);
            EXPECT_NE(type, cSourceActionTypes::SWITCH_UNDEF2);

            EXPECT_NE(type, cSourceActionTypes::PERIODIC_UNDEF);
            EXPECT_NE(type, cSourceActionTypes::PERIODIC_UNDEF2);

            EXPECT_LT(type, cSourceActionTypes::totalMaxWithInvalid);
        }
    }
}

// Check all actions are generated for on
TEST(TEST_SOURCEACTIONS, SWITCH_ON_COMPLETE) {
    cSourceJsonParamApi paramApi;
    paramApi.setOn(true);
    tSourceActionTypeList actionList = cSourceActionGenerator::generateSwitchActions(cSourceJsonParamApi(paramApi));
    EXPECT_EQ(actionList.count(), cSourceActionTypes::switchTypeCount);
}

template<class T> bool elementIsIn(T ele, std::vector<T> list){
    if(list.empty()){
        return false;
    }
    if(std::find(begin(list),end(list),ele) != list.end()){
        return true;
    }else{
        return false;
    }
}

// Check for switch on all valid
TEST(TEST_SOURCEACTIONS, SWITCH_ON_VALID) {
    cSourceJsonParamApi paramApi;
    paramApi.setOn(true);
    tSourceActionTypeList actionList = cSourceActionGenerator::generateSwitchActions(cSourceJsonParamApi(paramApi));
    for(auto action : actionList) {
        EXPECT_TRUE(elementIsIn<cSourceActionTypes::ActionTypes>(action,{
                                                                     cSourceActionTypes::SET_RMS,
                                                                     cSourceActionTypes::SET_ANGLE,
                                                                     cSourceActionTypes::SET_FREQUENCY,
                                                                     cSourceActionTypes::SET_HARMONICS,
                                                                     cSourceActionTypes::SET_REGULATOR,
                                                                     cSourceActionTypes::SWITCH_PHASES
                                                                 }));
        EXPECT_TRUE(cSourceActionTypes::isValidType(action));
    }
}

// Check for switch off just generates switch phases
TEST(TEST_SOURCEACTIONS, SWITCH_OFF_PHASE_ONLY) {
    cSourceJsonParamApi paramApi;
    paramApi.setOn(false);
    tSourceActionTypeList actionList = cSourceActionGenerator::generateSwitchActions(paramApi);
    EXPECT_EQ(actionList.count(), 1);
    EXPECT_EQ(actionList[0], cSourceActionTypes::ActionTypes::SWITCH_PHASES);
}

// Check for off are valid
TEST(TEST_SOURCEACTIONS, SWITCH_OFF_VALID) {
    cSourceJsonParamApi paramApi;
    paramApi.setOn(false);
    tSourceActionTypeList actionList = cSourceActionGenerator::generateSwitchActions(cSourceJsonParamApi(paramApi));
    for(auto action : actionList) {
        EXPECT_TRUE(elementIsIn<cSourceActionTypes::ActionTypes>(action,{
                                                                     cSourceActionTypes::SET_RMS,
                                                                     cSourceActionTypes::SET_ANGLE,
                                                                     cSourceActionTypes::SET_FREQUENCY,
                                                                     cSourceActionTypes::SET_HARMONICS,
                                                                     cSourceActionTypes::SET_REGULATOR,
                                                                     cSourceActionTypes::SWITCH_PHASES
                                                                 }));
        EXPECT_TRUE(cSourceActionTypes::isValidType(action));
    }
}

TEST(TEST_SOURCEACTIONS, PERIODIC_VALID) {
    tSourceActionTypeList actionList = cSourceActionGenerator::generatePeriodicActions();
    for(auto action : actionList) {
        EXPECT_TRUE(elementIsIn<cSourceActionTypes::ActionTypes>(action,{
                                                                     cSourceActionTypes::QUERY_STATUS,
                                                                     cSourceActionTypes::QUERY_ACTUAL
                                                                 }));
        EXPECT_TRUE(cSourceActionTypes::isValidType(action));
    }
}

TEST(TEST_SOURCEACTIONS, PERIODIC_COMPLETE) {
    tSourceActionTypeList actionList = cSourceActionGenerator::generatePeriodicActions();
    EXPECT_EQ(actionList.count(), cSourceActionTypes::periodicTypeCount);
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
