#ifndef VEINDATACOLLECTOR_H
#define VEINDATACOLLECTOR_H

#include <ve_storagesystem.h>
#include <timerperiodicqt.h>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>

class VeinDataCollector : public QObject
{
    Q_OBJECT
public:
    explicit VeinDataCollector(VeinEvent::StorageSystem* storage);
    void startLogging(QHash<int, QStringList> entitesAndComponents);
    void stopLogging();

signals:
    void newStoredValue(QJsonObject jsonObject);

private slots:
    void appendValue(int entityId, QString componentName, QVariant value, const QDateTime &timestamp);

private:
    QJsonObject convertToJson(QString timestamp, QHash<int, QHash<QString, QVariant> > infosHash);
    QJsonObject convertHashToJsonObject(QHash<QString, QVariant> hash);
    QJsonObject getJsonWithoutTimestamp(QString timestamp);
    QHash<QString, QVariant> appendNewValueToExistingValues(QJsonValue existingValue, QHash<QString, QVariant> compoValuesHash);
    void clearJson();

    VeinEvent::StorageSystem* m_storage;
    QHash<int, QStringList> m_componentsKeeper;
    QJsonObject m_jsonObject;
    TimerTemplateQtPtr m_periodicTimer;
};

#endif // VEINDATACOLLECTOR_H
