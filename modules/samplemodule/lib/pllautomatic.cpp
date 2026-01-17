#include "pllautomatic.h"

PllAutomatic::PllAutomatic(VeinStorage::AbstractDatabase *veinStorageDb, const QStringList &channelMNameList) :
    m_channelMNameList(channelMNameList),
    m_valueObserver(veinStorageDb)
{
    connect(&m_valueObserver, &RangeModuleValueObserver::sigNewValues,
            this, &PllAutomatic::onNewValues);
}

void PllAutomatic::activate(bool on)
{
    m_active = on;
    tryEmitPllChannel();
}

void PllAutomatic::onNewValues()
{
    tryEmitPllChannel();
}

void PllAutomatic::tryEmitPllChannel()
{
    if(!m_active)
        return;

    for (const QString &channelMName : qAsConst(m_channelMNameList)) {
        const float channelLevel = m_valueObserver.getRelativeRangeValue(channelMName);
        if(channelLevel > RelativeAmplitudeMinForPllChannelSelection) {
            emit sigSelectPllChannel(channelMName);
            return;
        }
    }
    tryEmitDefaultPllChannel();
}

void PllAutomatic::tryEmitDefaultPllChannel()
{
    if(!m_channelMNameList.isEmpty()) {
        emit sigSelectPllChannel(m_channelMNameList[0]);
        return;
    }
    qWarning("PllAutomatic has empty channel list!");
}

