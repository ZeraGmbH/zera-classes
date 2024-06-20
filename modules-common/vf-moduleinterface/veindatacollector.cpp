#include "veindatacollector.h"

 VeinDataCollector::VeinDataCollector(VeinEvent::StorageSystem *storage)
    :m_storage{storage}
{
}

void VeinDataCollector::collectRMSValues()
{
    int RMSEntity = 1040;
    VeinEvent::StorageComponentInterfacePtr rms1Component = m_storage->getFutureComponent(RMSEntity, "ACT_RMSPN1");
    connect(rms1Component.get(), &VeinEvent::StorageComponentInterface::sigValueChange, this, [&, RMSEntity](QVariant newValue) {
        VeinDataCollector::appendValue(RMSEntity, "ACT_RMSPN1", newValue);
    });
    VeinEvent::StorageComponentInterfacePtr rms2Component = m_storage->getFutureComponent(RMSEntity, "ACT_RMSPN2");
    connect(rms2Component.get(), &VeinEvent::StorageComponentInterface::sigValueChange, this, [&, RMSEntity](QVariant newValue) {
        VeinDataCollector::appendValue(RMSEntity, "ACT_RMSPN2", newValue);
    });
    VeinEvent::StorageComponentInterfacePtr rms3Component = m_storage->getFutureComponent(RMSEntity, "ACT_RMSPN3");
    connect(rms3Component.get(), &VeinEvent::StorageComponentInterface::sigValueChange, this, [&, RMSEntity](QVariant newValue) {
        VeinDataCollector::appendValue(RMSEntity, "ACT_RMSPN3", newValue);
    });
}

void VeinDataCollector::collectPowerValuesForEmobAC()
{
    int PowerEntity = 1070;
    VeinEvent::StorageComponentInterfacePtr power1Component = m_storage->getFutureComponent(PowerEntity, "ACT_PQS1");
    connect(power1Component.get(), &VeinEvent::StorageComponentInterface::sigValueChange, this, [&, PowerEntity](QVariant newValue) {
        VeinDataCollector::appendValue(PowerEntity, "ACT_PQS1", newValue);
    });
    VeinEvent::StorageComponentInterfacePtr power2Component = m_storage->getFutureComponent(PowerEntity, "ACT_PQS2");
    connect(power2Component.get(), &VeinEvent::StorageComponentInterface::sigValueChange, this, [&, PowerEntity](QVariant newValue) {
        VeinDataCollector::appendValue(PowerEntity, "ACT_PQS2", newValue);
    });
    VeinEvent::StorageComponentInterfacePtr power3Component = m_storage->getFutureComponent(PowerEntity, "ACT_PQS3");
    connect(power3Component.get(), &VeinEvent::StorageComponentInterface::sigValueChange, this, [&, PowerEntity](QVariant newValue) {
        VeinDataCollector::appendValue(PowerEntity, "ACT_PQS3", newValue);
    });
    VeinEvent::StorageComponentInterfacePtr powerSumComponent = m_storage->getFutureComponent(PowerEntity, "ACT_PQS4");
    connect(powerSumComponent.get(), &VeinEvent::StorageComponentInterface::sigValueChange, this, [&, PowerEntity](QVariant newValue) {
        VeinDataCollector::appendValue(PowerEntity, "ACT_PQS4", newValue);
    });
}

void VeinDataCollector::collectPowerValuesForEmobDC()
{
    int PowerEntity = 1073;
    VeinEvent::StorageComponentInterfacePtr power1Component = m_storage->getFutureComponent(PowerEntity, "ACT_PQS1");
    connect(power1Component.get(), &VeinEvent::StorageComponentInterface::sigValueChange, this, [&, PowerEntity](QVariant newValue) {
        VeinDataCollector::appendValue(PowerEntity, "ACT_PQS1", newValue);
    });
}

QHash<QString, QList<QVariant> > VeinDataCollector::getStoredValuesOfEntity(int entityId)
{
    return m_veinValuesHash[entityId];
}

void VeinDataCollector::appendValue(int entityId, QString componentName, QVariant value)
{
    m_veinValuesHash[entityId][componentName].append(value);
    convertToJson();
}

void VeinDataCollector::convertToJson()
{
    QJsonObject jsonObject;
    for (auto it = m_veinValuesHash.constBegin(); it != m_veinValuesHash.constEnd(); ++it) {
        jsonObject.insert(QString::number(it.key()), convertHashToJsonObject(it.value()));
    }
    emit newStoredValue(jsonObject);
}

QJsonArray VeinDataCollector::convertListToJsonArray(QList<QVariant> list)
{
    QJsonArray jsonArray;
    for (const QVariant& item : list) {
        jsonArray.append(QJsonValue::fromVariant(item));
    }
    return jsonArray;
}

QJsonObject VeinDataCollector::convertHashToJsonObject(QHash<QString, QList<QVariant> > hash)
{
    QJsonObject jsonObject;
    for (auto it = hash.constBegin(); it != hash.constEnd(); ++it) {
        jsonObject.insert(it.key(), convertListToJsonArray(it.value()));
    }
    return jsonObject;
}
