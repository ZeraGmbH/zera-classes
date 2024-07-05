#ifndef VF_STORAGE_H
#define VF_STORAGE_H

#include "veindatacollector.h"
#include <vf-cpp-entity.h>
#include <vf-cpp-compproxy.h>
#include <QObject>

class Vf_Storage : public QObject
{
    Q_OBJECT
public:
    explicit Vf_Storage(VeinEvent::StorageSystem* storageSystem, QObject *parent = nullptr, int entityId = 1);
    bool initOnce();
    bool addComponent(const QString componentName);
    VfCpp::VfCppEntity *getVeinEntity() const;
    void setVeinEntity(VfCpp::VfCppEntity *entity);

private:
    void startStopLogging(QVariant value, int storageNum);
    void readJson(QVariant value, int storageNum);
    QHash<int, QStringList> extractEntitiesAndComponents(QJsonObject jsonObject);
    void ignoreComponents(QStringList *componentList);

    VeinEvent::StorageSystem* m_storageSystem;
    VfCpp::VfCppEntity *m_entity;
    bool m_isInitalized;
    VfCpp::VfCppComponent::Ptr m_maximumLoggingComponents;
    QList<VfCpp::VfCppComponent::Ptr> m_storedValues;
    QList<VfCpp::VfCppComponent::Ptr> m_JsonWithEntities;
    QList<VfCpp::VfCppComponent::Ptr> m_startStopLogging;

    QList<VeinDataCollector*> m_dataCollect; //unique ptr ?
};

#endif // VF_STORAGE_H
