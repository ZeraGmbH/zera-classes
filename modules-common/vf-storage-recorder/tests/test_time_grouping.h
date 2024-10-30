#ifndef TEST_TIME_GROUPING_H
#define TEST_TIME_GROUPING_H

#include <QObject>

class test_time_grouping : public QObject
{
    Q_OBJECT
private slots:
    void oneTimestampGrouped();
    void twoTimestampsSeparate();
    void twoTimestampsGroupedSameEntity();
    void twoTimestampsGroupedDifferentEntity();
private:
    QJsonObject getStoredValueFromTimeStampIndex(QJsonObject newStoredValues, int timeIndex);
    QVariant getComponentValue(int entityId, QString componentName, QJsonObject storedValueWithoutTimeStamp);
};

#endif // TEST_TIME_GROUPING_H
