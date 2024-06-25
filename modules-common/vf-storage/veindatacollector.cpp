#include "veindatacollector.h"
#include <QDateTime>

 VeinDataCollector::VeinDataCollector(VeinEvent::StorageSystem *storage)
    :m_storage{storage}
{
}

void VeinDataCollector::startLogging(QHash<int, QStringList> entitesAndComponents)
{
    m_veinValuesHash.clear();
    for(int& entityId: entitesAndComponents.keys()) {
        QStringList components = entitesAndComponents[entityId];
        for(QString& component: components) {
            VeinEvent::StorageComponentInterfacePtr futureComponent = m_storage->getFutureComponent(entityId, component);
            connect(futureComponent.get(), &VeinEvent::StorageComponentInterface::sigValueChange, this, [&, component, entityId](QVariant newValue) {
                VeinDataCollector::appendValue(entityId, component, newValue);
            });
        }
    }
}

void VeinDataCollector::stopLogging()
{
    for(int& entityId: m_veinValuesHash.keys()) {
        QHash<QString, QList<QVariant>> compoAndValues = m_veinValuesHash[entityId];
        for(QString& component : compoAndValues.keys()) {
            VeinEvent::StorageComponentInterfacePtr futureComponent = m_storage->getFutureComponent(entityId, component);
            futureComponent->disconnect(SIGNAL(sigValueChange(QVariant)));
        }
    }
    m_veinValuesHash.clear();
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

QJsonObject VeinDataCollector::convertHashToJsonObject(QHash<QString, QList<QVariant> > hash)
{
    QJsonObject jsonObject;
    for (auto it = hash.constBegin(); it != hash.constEnd(); ++it) {
        jsonObject.insert(it.key(), convertListToJsonArray(it.value()));
    }
    return jsonObject;
}

QJsonArray VeinDataCollector::convertListToJsonArray(QList<QVariant> list)
{
    QJsonArray jsonArray;
    for (const QVariant& item : list) {
        QJsonObject itemObject;
        QDateTime now = QDateTime::currentDateTime();
        itemObject["time"] = now.toString("dd-MM-yyyy hh:mm:ss");
        itemObject["value"] = QJsonValue::fromVariant(item);
        jsonArray.append(itemObject);
    }
    return jsonArray;
}
