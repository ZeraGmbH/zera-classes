#ifndef VEINDATACOLLECTOR_H
#define VEINDATACOLLECTOR_H

#include <vs_abstracteventsystem.h>
#include <vs_timestampersettable.h>
#include <QJsonObject>
#include <QObject>
#include <QUuid>

typedef QHash<QString/*componentName*/, QVariant/*value*/> ComponentInfo;
typedef QHash<int/*entityId*/, ComponentInfo> RecordedEntityComponents;
typedef QMap<QString /*QDateTime in QString*/, RecordedEntityComponents> TimeStampedRecords;

static constexpr int sigMeasuringEntityId = 1050; //DftModule

class VeinDataCollector : public QObject
{
    Q_OBJECT
public:
    explicit VeinDataCollector(VeinStorage::AbstractDatabase* storage);
    void startLogging(QHash<int, QStringList> entitesAndComponents);
    void stopLogging();
    void getStoredValues(QUuid callId, int start, int end);
signals:
    void newValueStored(int num);
    void sigStoredValue(QUuid callId, bool success, QString errorMsg, QJsonObject values);

private:
    void collectValues(QDateTime timeStamp, QHash<int, QStringList> entitesAndComponents);
    QJsonObject convertRecordedEntityComponentsToJson(RecordedEntityComponents recordedEntityComponents);
    void appendNewRecord(QJsonObject newRecordObject);

    VeinStorage::AbstractDatabase *m_storage;
    QJsonObject m_recordedObject;
    VeinStorage::TimeStamperSettablePtr m_timeStamper;
    VeinStorage::AbstractComponentPtr m_sigMeasuringCompo;
};

#endif // VEINDATACOLLECTOR_H
