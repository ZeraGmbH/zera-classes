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
            connect(futureComponent.get(), &VeinEvent::StorageComponentInterface::sigValueChange, this, [&, component, entityId, futureComponent](QVariant newValue) {
                QString time = futureComponent->getTime();
                VeinDataCollector::appendValue(entityId, component, newValue, time);
            });
        }
    }
}

void VeinDataCollector::stopLogging()
{
    for(int& entityId: m_veinValuesHash.keys()) {
        QHash<QString, QVariant> compoAndValues = m_veinValuesHash[entityId];
        for(QString& component : compoAndValues.keys()) {
            VeinEvent::StorageComponentInterfacePtr futureComponent = m_storage->getFutureComponent(entityId, component);
            futureComponent->disconnect(SIGNAL(sigValueChange(QVariant)));
        }
    }
    m_veinValuesHash.clear();
}

void VeinDataCollector::appendValue(int entityId, QString componentName, QVariant value, QString time)
{
    m_veinValuesHash[entityId][componentName] = value;
    convertToJsonWithTimeStamp(time);
}

void VeinDataCollector::convertToJsonWithTimeStamp(QString time)
{
    m_jsonObject.insert(time, convertToJson());
    emit newStoredValue(m_jsonObject);
}

QJsonObject VeinDataCollector::convertToJson()
{
    QJsonObject jsonObject;
    for (auto it = m_veinValuesHash.constBegin(); it != m_veinValuesHash.constEnd(); ++it) {
        jsonObject.insert(QString::number(it.key()), convertHashToJsonObject(it.value()));
    }
    return jsonObject;
}

QJsonObject VeinDataCollector::convertHashToJsonObject(QHash<QString, QVariant> hash)
{
    QJsonObject jsonObject;
    for (auto it = hash.constBegin(); it != hash.constEnd(); ++it) {
        jsonObject.insert(it.key(), it.value().toString());
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
