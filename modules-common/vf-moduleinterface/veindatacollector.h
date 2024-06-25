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
    void collectRMSValues();
    void collectPowerValuesForEmobAC();
    void collectPowerValuesForEmobDC();
    QHash<QString, QList<QVariant>> getStoredValuesOfEntity(int entityId);

signals:
    void newStoredValue(QJsonObject jsonObject);

private slots:
    void appendValue(int entityId, QString componentName, QVariant value);

private:
    void convertToJson();
    QJsonObject convertHashToJsonObject(QHash<QString, QList<QVariant> > hash);
    QJsonArray convertListToJsonArray(QList<QVariant> list);

    VeinEvent::StorageSystem* m_storage;
    QHash<int , QHash<QString, QList<QVariant>> > m_veinValuesHash;
};

#endif // VEINDATACOLLECTOR_H
