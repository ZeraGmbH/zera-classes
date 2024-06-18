#ifndef VEINDATACOLLECTOR_H
#define VEINDATACOLLECTOR_H

#include <QVariant>
#include <QObject>
#include <ve_storagesystem.h>

class VeinDataCollector : public QObject
{
    Q_OBJECT
public:
    explicit VeinDataCollector(VeinEvent::StorageSystem* storage);
    void collectRMSValues();
    void collectPowerValuesForEmobAC();
    void collectPowerValuesForEmobDC();

    QHash<QString, QList<QVariant>> getStoredValuesOfEntity(int entityId);

private slots:
    void appendValue(int entityId, QString componentName, QVariant value);

private:
    VeinEvent::StorageSystem* m_storage;
    QHash<int , QHash<QString, QList<QVariant>> > m_veinValuesHash;
};

#endif // VEINDATACOLLECTOR_H
