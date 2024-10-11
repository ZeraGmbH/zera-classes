#ifndef VEINDATACOLLECTOR_H
#define VEINDATACOLLECTOR_H

#include <ve_storagesystem.h>
#include <veinstoragefilter.h>
#include <timerperiodicqt.h>
#include <QJsonObject>
#include <QDateTime>

class VeinDataCollector : public QObject
{
    Q_OBJECT
public:
    explicit VeinDataCollector(VeinEvent::StorageSystem* storage);
    void startLogging(QHash<int, QStringList> entitesAndComponents);
    void stopLogging();
signals:
    // Ideas:
    // * replace internal data QJsoonObject by
    //   typedef QHash<int/*entityId*/, QHash<QString/*componentName*/, QVariant/*value*/>> RecordedGroups;
    //   typedef QMap<qint64 /* msSinceEpochTimestamp */, RecordedGroups> TimeStampedGroups;
    // * split up filter / datacollection / periodic vein update into smaller pieces
    void newStoredValue(QJsonObject jsonObject);

private slots:
    void appendValue(int entityId, QString componentName, QVariant value, QDateTime timestamp);
private:
    QJsonObject convertToJson(QString timestamp, QHash<int, QHash<QString, QVariant> > infosHash);
    QJsonObject convertHashToJsonObject(QHash<QString, QVariant> hash);
    QJsonObject getJsonForTimestamp(QString timestamp);
    QHash<QString, QVariant> appendNewValueToExistingValues(QJsonValue existingValue, QHash<QString, QVariant> compoValuesHash);
    void clearJson();

    VeinStorageFilter m_storageFilter;
    QJsonObject m_jsonObject;
    TimerTemplateQtPtr m_periodicTimer;
};

#endif // VEINDATACOLLECTOR_H
