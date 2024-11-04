#include "veindatacollector.h"
#include "jsontimegrouping.h"
#include <timerfactoryqt.h>
#include <QDateTime>

// Note:
// StorageFilter::Settings are going to change - current settings are set
// to make tests happy

 VeinDataCollector::VeinDataCollector(VeinStorage::AbstractEventSystem *storage) :
    m_storageFilter(storage, VeinStorage::StorageFilter::Settings(false, true))
{
    connect(&m_storageFilter, &VeinStorage::StorageFilter::sigComponentValue,
            this, &VeinDataCollector::appendValue);

    m_periodicTimer = TimerFactoryQt::createPeriodic(100);
    connect(m_periodicTimer.get(), &TimerTemplateQt::sigExpired, this, &VeinDataCollector::onPeriodicTimerExpired);
}

void VeinDataCollector::startLogging(QHash<int, QStringList> entitesAndComponents)
{
    m_timestampedGrp.clear();
    m_regrouppedTimestampJson = QJsonObject();
    m_periodicTimer->start();
    for(auto iter=entitesAndComponents.cbegin(); iter!=entitesAndComponents.cend(); ++iter) {
        const QStringList components = iter.value();
        int entityId = iter.key();
        for(const QString& componentName : components)
            m_storageFilter.add(entityId, componentName);
    }
}

void VeinDataCollector::stopLogging()
{
    m_periodicTimer->stop();
    m_storageFilter.clear();
}

void VeinDataCollector::appendValue(int entityId, QString componentName, QVariant value, QDateTime timestamp)
{
    m_appendValueElapsedTimer.start();
    RecordedGroups newRecordedGrp;
    newRecordedGrp[entityId][componentName] = value;
    if(m_timestampedGrp.contains(timestamp.toMSecsSinceEpoch())) {
        RecordedGroups preRecordedGrp = m_timestampedGrp[timestamp.toMSecsSinceEpoch()];
        newRecordedGrp = TimeGrouping::appendEntityToRecordedGroup(preRecordedGrp, newRecordedGrp, entityId);
    }
    m_timestampedGrp.insert(timestamp.toMSecsSinceEpoch(), newRecordedGrp);
    qInfo("VeinDataCollector::appendValue took %llims", m_appendValueElapsedTimer.elapsed());
}

void VeinDataCollector::onPeriodicTimerExpired()
{
    QJsonObject newlyGroupedJson = TimeGrouping::regroupTimestamp(m_timestampedGrp);
    for (auto key: newlyGroupedJson.keys())
        m_regrouppedTimestampJson.insert(key, newlyGroupedJson.value(key));

    m_timestampedGrp.clear();
    emit newStoredValue(m_regrouppedTimestampJson);
}

