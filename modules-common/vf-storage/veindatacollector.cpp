#include "veindatacollector.h"
#include <QDateTime>

 VeinDataCollector::VeinDataCollector(VeinEvent::StorageSystem *storage)
    :m_storage{storage}
{
}

void VeinDataCollector::startLogging(QHash<int, QStringList> entitesAndComponents)
{
    clearJson();
    for(int& entityId: entitesAndComponents.keys()) {
        QStringList components = entitesAndComponents[entityId];
        for(QString& component: components) {
            VeinEvent::StorageComponentInterfacePtr actualComponent = m_storage->getComponent(entityId, component);
            connect(actualComponent.get(), &VeinEvent::StorageComponentInterface::sigValueChange, this, [=](QVariant newValue) {
                QDateTime time = actualComponent->getTimestamp();
                VeinDataCollector::appendValue(entityId, component, newValue, time.toString("dd-MM-yyyy hh:mm:ss.zzz"));
            });
        }
    }
}

void VeinDataCollector::stopLogging()
{
    for(int& entityId: m_veinValuesHash.keys()) {
        QHash<QString, QVariant> compoAndValues = m_veinValuesHash[entityId];
        for(QString& component : compoAndValues.keys()) {
            VeinEvent::StorageComponentInterfacePtr actualComponent = m_storage->getComponent(entityId, component);
            actualComponent->disconnect(SIGNAL(sigValueChange(QVariant)));
        }
    }
    m_veinValuesHash.clear();
}

void VeinDataCollector::appendValue(int entityId, QString componentName, QVariant value, QString timestamp)
{
    m_veinValuesHash[entityId][componentName] = value;
    convertToJsonWithTimeStamp(timestamp);
}

void VeinDataCollector::convertToJsonWithTimeStamp(QString timestamp)
{
    m_jsonObject.insert(timestamp, convertToJson());
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

void VeinDataCollector::clearJson()
{
    if(!m_jsonObject.isEmpty()) {
        for (const QString &key : m_jsonObject.keys()) {
            m_jsonObject.remove(key);
        }
    }
}
