#include "test_time_grouping.h"
#include "timegrouping.h"
#include <QtTest>

QTEST_MAIN(test_time_grouping)

void test_time_grouping::oneTimestampGrouped()
{
    int entityID = 10;
    QHash<QString, QVariant> componentData{{"component1", "foo"}};
    RecordedGroups recordedGroup {{entityID, componentData}};
    TimeStampedGroups timeStampedGroup {{150, recordedGroup}};

    QJsonObject regroupedJson = TimeGrouping::regroupTimestamp(timeStampedGroup);

    QCOMPARE(regroupedJson.size(), 1);
    QCOMPARE(regroupedJson.keys().at(0), QDateTime::fromMSecsSinceEpoch(150).toString(TimeGrouping::DateTimeFormat));

    QJsonObject recordedGroupJson = getStoredValueFromTimeStampIndex(regroupedJson, 0);
    QCOMPARE(getComponentValue(entityID, "component1", recordedGroupJson), "foo");
}

void test_time_grouping::twoTimestampsSeparate()
{
    int entityID = 10;
    QHash<QString, QVariant> componentData1{{"component1", "foo"}};
    RecordedGroups recordedGroup1 {{entityID, componentData1}};
    TimeStampedGroups timeStampedGroup {{150, recordedGroup1}};

    QHash<QString, QVariant> componentData2{{"component1", "bar"}};
    RecordedGroups recordedGroup2 {{entityID, componentData2}};
    timeStampedGroup.insert(300, recordedGroup2);

    QJsonObject regroupedJson = TimeGrouping::regroupTimestamp(timeStampedGroup);

    QCOMPARE(regroupedJson.size(), 2);
    QCOMPARE(regroupedJson.keys().at(0), QDateTime::fromMSecsSinceEpoch(150).toString(TimeGrouping::DateTimeFormat));
    QCOMPARE(regroupedJson.keys().at(1), QDateTime::fromMSecsSinceEpoch(300).toString(TimeGrouping::DateTimeFormat));

    QJsonObject recordedGroupJson = getStoredValueFromTimeStampIndex(regroupedJson, 0);
    QCOMPARE(getComponentValue(entityID, "component1", recordedGroupJson), "foo");
    recordedGroupJson = getStoredValueFromTimeStampIndex(regroupedJson, 1);
    QCOMPARE(getComponentValue(entityID, "component1", recordedGroupJson), "bar");
}

void test_time_grouping::twoTimestampsGroupedSameEntitySameComponent()
{
    int entityID = 10;
    QHash<QString, QVariant> componentData1{{"component1", "foo"}};
    RecordedGroups recordedGroup1 {{entityID, componentData1}};
    TimeStampedGroups timeStampedGroup {{110, recordedGroup1}};

    QHash<QString, QVariant> componentData2{{"component1", "bar"}};
    RecordedGroups recordedGroup2 {{entityID, componentData2}};
    timeStampedGroup.insert(120, recordedGroup2);

    QJsonObject regroupedJson = TimeGrouping::regroupTimestamp(timeStampedGroup);

    QCOMPARE(regroupedJson.size(), 1);
    QCOMPARE(regroupedJson.keys().at(0), QDateTime::fromMSecsSinceEpoch(110).toString(TimeGrouping::DateTimeFormat));

    QJsonObject recordedGroupJson = getStoredValueFromTimeStampIndex(regroupedJson, 0);
    QCOMPARE(getComponentValue(entityID, "component1", recordedGroupJson), "bar");
}

void test_time_grouping::twoTimestampsGroupedDifferentEntity()
{
    int entityID1 = 1040;
    QHash<QString, QVariant> componentData1{{"RMSPN1", "230"}};
    RecordedGroups recordedGroup1 {{entityID1, componentData1}};
    TimeStampedGroups timeStampedGroup {{110, recordedGroup1}};

    QHash<QString, QVariant> componentData2{{"RMSPN5", "227"}};
    RecordedGroups recordedGroup2 {{entityID1, componentData2}};
    int entityID2 = 1070;
    QHash<QString, QVariant> componentData3{{"PQS1", "2000"}};
    recordedGroup2.insert(entityID2, componentData3);
    timeStampedGroup.insert(120, recordedGroup2);

    QJsonObject regroupedJson = TimeGrouping::regroupTimestamp(timeStampedGroup);

    QCOMPARE(regroupedJson.size(), 1);
    QCOMPARE(regroupedJson.keys().at(0), QDateTime::fromMSecsSinceEpoch(110).toString(TimeGrouping::DateTimeFormat));

    QJsonObject recordedGroupJson = getStoredValueFromTimeStampIndex(regroupedJson, 0);
    QCOMPARE(getComponentValue(entityID1, "RMSPN1", recordedGroupJson), "230");
    QCOMPARE(getComponentValue(entityID1, "RMSPN5", recordedGroupJson), "227");
    QCOMPARE(getComponentValue(entityID2, "PQS1", recordedGroupJson), "2000");
}

QJsonObject test_time_grouping::getStoredValueFromTimeStampIndex(QJsonObject newStoredValues, int timeIndex)
{
    QStringList timeStamps = newStoredValues.keys();
    return newStoredValues.value(timeStamps.at(timeIndex)).toObject();
}

QVariant test_time_grouping::getComponentValue(int entityId, QString componentName, QJsonObject storedValueWithoutTimeStamp)
{
    return storedValueWithoutTimeStamp.value(QString::number(entityId)).toObject().value(componentName);
}
