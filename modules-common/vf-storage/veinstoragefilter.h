#ifndef VEINSTORAGEFILTER_H
#define VEINSTORAGEFILTER_H

#include <ve_storagesystem.h>
#include <QDateTime>
#include <QList>
#include <QHash>

// A candidate for vein-framework once matured???
class VeinStorageFilter : public QObject
{
    Q_OBJECT
public:
    struct Settings {
        Settings(bool fireCurrentValidOnAddFiter, bool fireOnChangesOnly) :
            m_fireCurrentValidOnAddFiter(fireCurrentValidOnAddFiter),
            m_fireOnChangesOnly(fireOnChangesOnly)
            {}
        bool m_fireCurrentValidOnAddFiter;
        bool m_fireOnChangesOnly;
    };
    explicit VeinStorageFilter(VeinEvent::StorageSystem* storage, Settings settings);
    ~VeinStorageFilter();
    bool add(int entityId, QString componentName);
    void clear();
signals:
    void sigComponentValue(int entityId, QString componentName, QVariant value, QDateTime timestamp);

private:
    void fireActual(int entityId, QString componentName, VeinEvent::StorageComponentInterfacePtr actualComponent);
    const VeinEvent::StorageSystem* m_storage;
    const Settings m_settings;
    QList<QMetaObject::Connection> m_componentChangeConnections;
    QHash<int, QSet<QString>> m_filteredEntityComponents;
};

#endif // VEINSTORAGEFILTER_H
