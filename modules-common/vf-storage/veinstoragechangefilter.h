#ifndef VEINSTORAGECHANGEFILTER_H
#define VEINSTORAGECHANGEFILTER_H

#include <ve_storagesystem.h>
#include <QDateTime>
#include <QList>
#include <QHash>

// A candidate for vein-framework once matured???
class VeinStorageChangeFilter : public QObject
{
    Q_OBJECT
public:
    explicit VeinStorageChangeFilter(VeinEvent::StorageSystem* storage, bool fireCurrentValidOnAddFiter);
    ~VeinStorageChangeFilter();
    bool add(int entityId, QString componentName);
    void clear();
signals:
    void sigComponentValue(int entityId, QString componentName, QVariant value, QDateTime timestamp);

private:
    void fireActual(int entityId, QString componentName, VeinEvent::StorageComponentInterfacePtr actualComponent);
    const VeinEvent::StorageSystem* m_storage;
    const bool m_fireCurrentValidOnAddFiter;
    QList<QMetaObject::Connection> m_componentChangeConnections;
    QHash<int, QSet<QString>> m_filteredEntityComponents;
};

#endif // VEINSTORAGECHANGEFILTER_H
