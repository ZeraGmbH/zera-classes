#ifndef VEINDATACOLLECTOR_H
#define VEINDATACOLLECTOR_H

#include <ve_storagesystem.h>
#include <QJsonObject>
#include <QJsonArray>
#include <QObject>

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
    void appendValue(int entityId, QString componentName, QVariant value, QString timestamp);

private:
    void convertToJsonWithTimeStamp(QString timestamp, QHash<int, QHash<QString, QVariant> > infosHash);
    QJsonObject convertToJson(QString timestamp, QHash<int, QHash<QString, QVariant> > infosHash);
    QJsonObject convertHashToJsonObject(QHash<QString, QVariant> hash);
    void clearJson();

    VeinEvent::StorageSystem* m_storage;
    QHash<int, QStringList> m_componentsKeeper;
    QJsonObject m_jsonObject;
};

#endif // VEINDATACOLLECTOR_H
