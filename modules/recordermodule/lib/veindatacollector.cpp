#include "veindatacollector.h"

VeinDataCollector::VeinDataCollector(VeinStorage::AbstractDatabase *storage) :
    m_storage(storage)
{
    m_timeStamper = VeinStorage::TimeStamperSettable::create();
}

void VeinDataCollector::startLogging(QHash<int, QStringList> entitesAndComponents)
{
    m_recordedObject = QJsonObject();
    m_sigMeasuringCompo = m_storage->findComponent(sigMeasuringEntityId, "SIG_Measuring");
    if(m_sigMeasuringCompo) {
        connect(m_sigMeasuringCompo.get(), &VeinStorage::AbstractComponent::sigValueChange, this, [=](QVariant newValue){
            if(newValue.toInt() == 1) {// 1 indicates DftModule received new actual values
                m_timeStamper->setTimestampToNow();
                collectValues(m_timeStamper->getTimestamp(), entitesAndComponents);
                emit newValueStored(m_recordedObject.count());
            }
        });
    }
    else
        qInfo("DftModule/SIG_Measuring component is missing.");
}

void VeinDataCollector::stopLogging()
{
    disconnect(m_sigMeasuringCompo.get(), 0, this, 0);
}

void VeinDataCollector::getStoredValues(QUuid callId, int start, int end)
{
    if(start < end && end <= m_recordedObject.count()) {
        QJsonObject object;
        int index = 0;
        for(auto it = m_recordedObject.begin(); it != m_recordedObject.end(); ++it) {
            if(index >= start && index <= end)
                object.insert(it.key(), it.value());
            index++;
        }
        emit sigStoredValue(callId, true, "", object);
    }
    else
        emit sigStoredValue(callId, false, "start or end not matching", QJsonObject());
}

void VeinDataCollector::collectValues(QDateTime timeStamp, QHash<int, QStringList> entitesAndComponents)
{
    RecordedEntityComponents newRecord;
    for(auto entity: entitesAndComponents.keys()) {
        ComponentInfo componentValues;
        for(auto componentName: entitesAndComponents.value(entity))
            componentValues.insert(componentName, m_storage->getStoredValue(entity, componentName));
        newRecord.insert(entity, componentValues);
    }

    QString timeString = timeStamp.toUTC().toString("dd-MM-yyyy hh:mm:ss.zzz");
    QJsonObject newRecordObject = QJsonObject{{timeString, convertRecordedEntityComponentsToJson(newRecord)}};
    appendNewRecord(newRecordObject);
}

QJsonObject VeinDataCollector::convertRecordedEntityComponentsToJson(RecordedEntityComponents recordedEntityComponents)
{
    QJsonObject json;
    for(auto entityID: recordedEntityComponents.keys()) {
        QJsonObject componentJson = QJsonObject::fromVariantHash(recordedEntityComponents[entityID]);
        QString entityIDToString = QString::number(entityID);
        json.insert(entityIDToString, componentJson);
    }
    return json;
}

void VeinDataCollector::appendNewRecord(QJsonObject newRecordObject)
{
    for (auto it = newRecordObject.begin(); it != newRecordObject.end(); ++it)
        m_recordedObject.insert(it.key(), it.value());
}
