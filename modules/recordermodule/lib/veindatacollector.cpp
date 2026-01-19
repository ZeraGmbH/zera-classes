#include "veindatacollector.h"
#include <timerfactoryqt.h>

VeinDataCollector::VeinDataCollector(VeinStorage::AbstractDatabase *storage) :
    m_storage(storage)
{
}

void VeinDataCollector::startLogging(const QHash<int, QStringList> &entitesAndComponents)
{
    VeinStorage::AbstractComponentPtr sigMeasuringComponent = getMeasSigComponent();
    if (sigMeasuringComponent == nullptr) {
        qWarning("DftModule/SIG_Measuring component is missing.");
        return;
    }

    m_entitesAndComponentsLogged = entitesAndComponents;
    m_recordedObject = QJsonObject();
    m_timeStampMsFirstRecording = notStartedFirstTimestampValue;
    connect(sigMeasuringComponent.get(), &VeinStorage::AbstractComponent::sigValueChange,
            this, &VeinDataCollector::onMeasuringComponentChanged);
}

void VeinDataCollector::stopLogging()
{
    VeinStorage::AbstractComponentPtr sigMeasuringComponent = getMeasSigComponent();
    if (sigMeasuringComponent != nullptr)
        disconnect(sigMeasuringComponent.get(), &VeinStorage::AbstractComponent::sigValueChange,
                   this, &VeinDataCollector::onMeasuringComponentChanged);
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

void VeinDataCollector::onMeasuringComponentChanged(QVariant newValue)
{
    if(newValue.toInt() == 1) {// 1 indicates DftModule received new actual values
        qint64 nowMs = TimerFactoryQt::getCurrentTime().toMSecsSinceEpoch();
        if (m_timeStampMsFirstRecording == notStartedFirstTimestampValue)
            m_timeStampMsFirstRecording = nowMs;
        qint64 msSinceStart = nowMs - m_timeStampMsFirstRecording;
        collectValues(msSinceStart);
        emit newValueStored(m_recordedObject.count());
    }
}

void VeinDataCollector::collectValues(int msSinceStart)
{
    RecordedEntityComponents newRecord;
    for(auto entity : m_entitesAndComponentsLogged.keys()) {
        ComponentInfo componentValues;
        for(const QString &componentName : m_entitesAndComponentsLogged.value(entity))
            componentValues.insert(componentName, m_storage->getStoredValue(entity, componentName));
        newRecord.insert(entity, componentValues);
    }

    QString timeDiffSecStr = intToStringWithLeadingDigits(msSinceStart);
    m_recordedObject.insert(timeDiffSecStr, convertRecordedEntityComponentsToJson(newRecord));
}

QJsonObject VeinDataCollector::convertRecordedEntityComponentsToJson(RecordedEntityComponents recordedEntityComponents)
{
    QJsonObject json;
    for(auto entityID : recordedEntityComponents.keys()) {
        QJsonObject componentJson = QJsonObject::fromVariantHash(recordedEntityComponents[entityID]);
        QString entityIDToString = QString::number(entityID);
        json.insert(entityIDToString, componentJson);
    }
    return json;
}

VeinStorage::AbstractComponentPtr VeinDataCollector::getMeasSigComponent()
{
    return m_storage->findComponent(sigMeasuringEntityId, "SIG_Measuring");;
}

QString VeinDataCollector::intToStringWithLeadingDigits(int number)
{
    return QString("0000000000%1").arg(QString::number(number)).right(10);
}
