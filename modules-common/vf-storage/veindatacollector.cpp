#include "veindatacollector.h"
#include "jsontimegrouping.h"
#include <timerfactoryqt.h>
#include <QDateTime>

 VeinDataCollector::VeinDataCollector(VeinEvent::StorageSystem *storage)
    :m_storage{storage}
{
    m_periodicTimer = TimerFactoryQt::createPeriodic(100);
    connect(m_periodicTimer.get(), &TimerTemplateQt::sigExpired,this, [&]{
        emit newStoredValue(JsonTimeGrouping::regroupTimestamp(m_jsonObject));
    });
}

void VeinDataCollector::startLogging(QHash<int, QStringList> entitesAndComponents)
{
    clearJson();
    m_periodicTimer->start();
    for(auto iter=entitesAndComponents.cbegin(); iter!=entitesAndComponents.cend(); ++iter) {
        const QStringList components = iter.value();
        int entityId = iter.key();
        for(const QString& component : components) {
            VeinEvent::StorageComponentInterfacePtr actualComponent = m_storage->getComponent(entityId, component);
            auto conn = connect(actualComponent.get(), &VeinEvent::StorageComponentInterface::sigValueChange, this, [=](QVariant newValue) {
                appendValue(entityId, component, newValue, actualComponent->getTimestamp());
            });
            m_componentChangeConnections.append(conn);
        }
    }
}

void VeinDataCollector::stopLogging()
{
    m_periodicTimer->stop();
    for(auto &conn : m_componentChangeConnections)
        disconnect(conn);
    m_componentChangeConnections.clear();
}

void VeinDataCollector::appendValue(int entityId, QString componentName, QVariant value, const QDateTime &timestamp)
{
    QHash<int , QHash<QString, QVariant> > infosHash;
    infosHash[entityId][componentName] = value;
    QString timeString = timestamp.toString("dd-MM-yyyy hh:mm:ss.zzz");
    m_jsonObject.insert(timeString, convertToJson(timeString, infosHash));
}

QJsonObject VeinDataCollector::convertToJson(QString timestamp, QHash<int , QHash<QString, QVariant>> infosHash)
{
    QJsonObject jsonObject = getJsonForTimestamp(timestamp);
    for(auto it = infosHash.constBegin(); it != infosHash.constEnd(); ++it) {
        QString entityIdToString = QString::number(it.key());
        if(jsonObject.contains(entityIdToString)) {
            QJsonValue existingValue = jsonObject.value(entityIdToString);
            QHash<QString, QVariant> hash = appendNewValueToExistingValues(existingValue, it.value()) ;
            jsonObject.insert(entityIdToString, convertHashToJsonObject(hash));
        }
        else
            jsonObject.insert(entityIdToString, convertHashToJsonObject(it.value()));
    }
    return jsonObject;
}

QJsonObject VeinDataCollector::getJsonForTimestamp(QString timestamp)
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
