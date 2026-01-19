#ifndef VEINDATACOLLECTOR_H
#define VEINDATACOLLECTOR_H

#include <vs_abstracteventsystem.h>
#include <QJsonObject>
#include <QObject>
#include <QUuid>

typedef QHash<QString/*componentName*/, QVariant/*value*/> ComponentInfo;
typedef QHash<int/*entityId*/, ComponentInfo> RecordedEntityComponents;
typedef QMap<QString /*QDateTime in QString*/, RecordedEntityComponents> TimeStampedRecords;

static constexpr int sigMeasuringEntityId = 1050; //DftModule
static constexpr qint64 notStartedFirstTimestampValue = -1;

class VeinDataCollector : public QObject
{
    Q_OBJECT
public:
    explicit VeinDataCollector(VeinStorage::AbstractDatabase* storage);
    void startLogging(const QHash<int, QStringList> &entitesAndComponents);
    void stopLogging();
    void getStoredValues(QUuid callId, int start, int end);
    static QString intToStringWithLeadingDigits(int number);
signals:
    void newValueStored(int num);
    void sigStoredValue(QUuid callId, bool success, QString errorMsg, QJsonObject values);

private slots:
    void onMeasuringComponentChanged(QVariant newValue);
private:
    void collectValues(int msSinceStart);
    QJsonObject convertRecordedEntityComponentsToJson(RecordedEntityComponents recordedEntityComponents);
    VeinStorage::AbstractComponentPtr getMeasSigComponent();

    VeinStorage::AbstractDatabase *m_storage;
    QJsonObject m_recordedObject;
    QHash<int, QStringList> m_entitesAndComponentsLogged;
    qint64 m_timeStampMsFirstRecording = notStartedFirstTimestampValue;
};

#endif // VEINDATACOLLECTOR_H
