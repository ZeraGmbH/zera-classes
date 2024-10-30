#ifndef TIMEGROUPING_H
#define TIMEGROUPING_H

#include <QJsonObject>
#include <QVariant>
#include <QMap>
#include <QHash>

typedef QHash<int/*entityId*/, QHash<QString/*componentName*/, QVariant/*value*/>> RecordedGroups;
typedef QMap<qint64 /* msSinceEpochTimestamp */, RecordedGroups> TimeStampedGroups;

class TimeGrouping
{
public:
    static const char* DateTimeFormat;
    static QJsonObject regroupTimestamp(TimeStampedGroups inputTimeStampedGroups);
    static RecordedGroups appendComponentInfoToRecordedGroup(RecordedGroups currentGroup, RecordedGroups inputGroup, int entityID);
    static RecordedGroups appendEntityToRecordedGroup(RecordedGroups currentGroup, RecordedGroups inputGroup, int entityID);
private:
    static QJsonObject convertTimeStampedGroupsToJson(TimeStampedGroups timeStampedGroups);
};

#endif // TIMEGROUPING_H
