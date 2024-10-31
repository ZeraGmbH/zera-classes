#include "timegrouping.h"
#include <QDateTime>
#include <QString>

const char* TimeGrouping::DateTimeFormat = "dd-MM-yyyy hh:mm:ss.zzz";
static const qint64 maxTimeDiffMs = 100;
QElapsedTimer TimeGrouping::m_regroupingTime;

QJsonObject TimeGrouping::regroupTimestamp(TimeStampedGroups inputTimeStampedGroups)
{
    m_regroupingTime.start();
    TimeStampedGroups regroupedTimeStamps;
    qint64 currentTimeStamp = 0;

    for(auto timeStamp: inputTimeStampedGroups.keys()) {
        if(!regroupedTimeStamps.contains(timeStamp)) {
            RecordedGroups inputRecordedGroup = inputTimeStampedGroups.value(timeStamp);
            if(timeStamp - currentTimeStamp > maxTimeDiffMs) {
                regroupedTimeStamps.insert(timeStamp, inputRecordedGroup);
                currentTimeStamp = timeStamp;
            }
            else {
                RecordedGroups preRecordedGroup = regroupedTimeStamps.value(currentTimeStamp);
                for(auto inputEntity: inputRecordedGroup.keys())
                    if (preRecordedGroup.contains(inputEntity))
                        regroupedTimeStamps.insert(currentTimeStamp, appendComponentInfoToRecordedGroup(preRecordedGroup, inputRecordedGroup, inputEntity));
                    else
                        regroupedTimeStamps.insert(currentTimeStamp, appendEntityToRecordedGroup(preRecordedGroup,inputRecordedGroup, inputEntity));
            }
        }
    }
    QJsonObject returnJson = convertTimeStampedGroupsToJson(regroupedTimeStamps);
    qInfo("TimeGrouping::regrouping took %llims", m_regroupingTime.elapsed());
    return returnJson;
}

RecordedGroups TimeGrouping::appendComponentInfoToRecordedGroup(RecordedGroups currentGroup, RecordedGroups inputGroup, int entityID)
{
    QVariantHash currentComponentInfo = currentGroup.value(entityID);
    QVariantHash inputComponentInfo = inputGroup.value(entityID);
    for(auto componentName : inputComponentInfo.keys())
        currentComponentInfo.insert(componentName, inputComponentInfo[componentName]);
    currentGroup.insert(entityID, currentComponentInfo);
    return currentGroup;
}

RecordedGroups TimeGrouping::appendEntityToRecordedGroup(RecordedGroups currentGroup, RecordedGroups inputGroup, int entityID)
{
    currentGroup.insert(entityID, inputGroup[entityID]);
    return currentGroup;
}

QJsonObject TimeGrouping::convertTimeStampedGroupsToJson(TimeStampedGroups timeStampedGroups)
{
    QJsonObject retJson;
    for(auto timestamp : timeStampedGroups.keys()) {
        QJsonObject entitiesJson;
        RecordedGroups recordedGrp = timeStampedGroups[timestamp];
        for(auto entityId : recordedGrp.keys()) {
            QVariantHash compoAndValue = recordedGrp[entityId];
            QJsonObject compoAndValueJson = QJsonObject::fromVariantHash(compoAndValue);
            entitiesJson.insert(QString::number(entityId), compoAndValueJson);
        }
        retJson.insert(QDateTime::fromMSecsSinceEpoch(timestamp).toString(DateTimeFormat), entitiesJson);
    }
    return retJson;
}

