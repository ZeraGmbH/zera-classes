#include "veindatacollector.h"
#include "jsontimegrouping.h"
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
            m_componentsKeeper[entityId].append(component);
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
    for(int& entityId: m_componentsKeeper.keys()) {
        QStringList components = m_componentsKeeper[entityId];
        for(QString& component : components) {
            VeinEvent::StorageComponentInterfacePtr actualComponent = m_storage->getComponent(entityId, component);
            actualComponent->disconnect(SIGNAL(sigValueChange(QVariant)));
        }
    }
    m_componentsKeeper.clear();
}

void VeinDataCollector::appendValue(int entityId, QString componentName, QVariant value, QString timestamp)
{
    QHash<int , QHash<QString, QVariant> > infosHash;
    infosHash[entityId][componentName] = value;
    convertToJsonWithTimeStamp(timestamp, infosHash);
}

void VeinDataCollector::convertToJsonWithTimeStamp(QString timestamp, QHash<int , QHash<QString, QVariant>> infosHash)
{
    m_jsonObject.insert(timestamp, convertToJson(timestamp, infosHash));
    JsonTimeGrouping jsonGrouping(m_jsonObject);
    emit newStoredValue(jsonGrouping.regroupTimestamp());
}

QJsonObject VeinDataCollector::convertToJson(QString timestamp, QHash<int , QHash<QString, QVariant>> infosHash)
{
    QJsonObject jsonObject;
    for (auto it = infosHash.constBegin(); it != infosHash.constEnd(); ++it) {
        QJsonObject jsonWithoutTimestamp = getJsonWithoutTimestamp(timestamp);
        QString entityIdToString = QString::number(it.key());

        if(jsonWithoutTimestamp.contains(entityIdToString) ) {
            QJsonValue existingValue = jsonWithoutTimestamp.value(entityIdToString);
            QHash<QString, QVariant> hash = appendNewValueToExistingValues(existingValue, it.value()) ;
            jsonObject = jsonWithoutTimestamp;
            jsonObject.insert(entityIdToString, convertHashToJsonObject(hash));
        }
        else {
            jsonObject = jsonWithoutTimestamp;
            jsonObject.insert(entityIdToString, convertHashToJsonObject(it.value()));
        }
    }
    return jsonObject;
}

QJsonObject VeinDataCollector::getJsonWithoutTimestamp(QString timestamp)
{
    QJsonObject jsonWithoutTimestamp;
    for(const QString &key : m_jsonObject.keys()) {
        if(key == timestamp)
            jsonWithoutTimestamp = m_jsonObject.value(key).toObject();
    }
    return jsonWithoutTimestamp;
}

QHash<QString, QVariant> VeinDataCollector::appendNewValueToExistingValues(QJsonValue existingValue, QHash<QString, QVariant> compoValuesHash)
{
    QHash<QString, QVariant> hash= existingValue.toObject().toVariantHash();
    for (auto hashIt = compoValuesHash.constBegin(); hashIt != compoValuesHash.constEnd(); ++hashIt)
        hash.insert(hashIt.key(), hashIt.value());
    return hash;
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
