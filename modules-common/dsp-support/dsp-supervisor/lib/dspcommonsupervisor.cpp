#include "dspcommonsupervisor.h"
#include <timerfactoryqt.h>

void DspCommonSupervisor::addSupervisorEntries(const QList<DspSupervisorInput> &entriesChronological, int maxEntries)
{
    if (entriesChronological.isEmpty())
        return;

    const DspSupervisorInput &lastEntry = entriesChronological.last();
    const quint32 lastMs = lastEntry.m_msTimer;
    const QDateTime now = TimerFactoryQt::getCurrentTime();

    for (const DspSupervisorInput &entryIn : entriesChronological) {
        DspSupervisorOutput entryOut;
        entryOut.m_rawIn = entryIn;
        quint32 relTimeMs = lastMs - entryIn.m_msTimer;
        entryOut.m_timeStamp = now.addMSecs(-qint64(relTimeMs));

        quint32 currPeriod = entryIn.m_periodCount;
        m_supervisorMap[currPeriod] = entryOut;

        m_entryStore.append(currPeriod);
    }

    while (m_entryStore.count() > maxEntries) {
        quint32 obsoletePeriod = m_entryStore.takeFirst();
        m_supervisorMap.remove(obsoletePeriod);
    }

    m_currentPeriod = lastEntry.m_periodCount;
    m_currentMsTime = lastEntry.m_msTimer;
}

const DspSupervisorOutputMap &DspCommonSupervisor::getSupervisorMap() const
{
    return m_supervisorMap;
}

quint32 DspCommonSupervisor::getCurrentPeriod() const
{
    return m_currentPeriod;
}

quint32 DspCommonSupervisor::getCurrentMsTime() const
{
    return m_currentMsTime;
}
