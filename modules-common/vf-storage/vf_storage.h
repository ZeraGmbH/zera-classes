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

private slots:
    void updateValue(QJsonObject value);
    void startStopLogging(QVariant value);

private:
    void readJson(QVariant value);
    QHash<int, QStringList> extractEntitiesAndComponents(QJsonObject jsonObject);

    VfCpp::VfCppEntity *m_entity;
    bool m_isInitalized;
    VfCpp::VfCppComponent::Ptr m_storedValues;
    VfCpp::VfCppComponent::Ptr m_JsonWithEntities;
    VfCpp::VfCppComponent::Ptr m_startStopLogging;

    VeinDataCollector *m_dataCollect; //unique ptr ?
};

#endif // VF_STORAGE_H
