#include "demodspinterfacedspsuper.h"
#include <timerfactoryqt.h>

DemoDspInterfaceDspSuper::DemoDspInterfaceDspSuper(int entityId,
                                                   std::function<double (int)> valueGenerator) :
    m_entityId(entityId),
    m_valueGenerator(valueGenerator),
    m_periodicTimer(TimerFactoryQt::createPeriodic(100)) // Mimics 10Hz period frequency
{
    connect(m_periodicTimer.get(), &TimerTemplateQt::sigExpired,
            this, &DemoDspInterfaceDspSuper::onTimer);
    connect(this, &DemoDspInterfaceDspSuper::sigDspStarted, [&](){
        m_periodicTimer->start();
    });
    sendInterrupt();
}

void DemoDspInterfaceDspSuper::onTimer()
{
    if (m_currPeriodCount >= m_nextInterruptPeriod)
        sendInterrupt();
    m_currPeriodCount++;
}

int DemoDspInterfaceDspSuper::genNextInterruptWithinPeriods() const
{
    int nextInterruptWithinPeriods = (m_valueGenerator(m_entityId)*3.5);
    if (nextInterruptWithinPeriods == 0)
        nextInterruptWithinPeriods = 1;
    return nextInterruptWithinPeriods;
}

void DemoDspInterfaceDspSuper::sendInterrupt()
{
    int periodsToGenerateValuesFor = m_currPeriodCount - m_lastInterruptPeriod;
    if (periodsToGenerateValuesFor > 0) {
        QVector<float> demoValues;
        for (int i=0; i<periodsToGenerateValuesFor; ++i) {
            int periodToStore = m_lastInterruptPeriod + i + 1;
            float busyValFlt = m_valueGenerator(m_entityId) * 15;
            demoValues.append(busyValFlt);

            float periodCountFlt = *reinterpret_cast<float*>(&periodToStore);
            demoValues.append(periodCountFlt);

            uint msTime = 20*periodToStore;
            float msTimeFlt = *reinterpret_cast<float*>(&msTime);
            demoValues.append(msTimeFlt);
        }
        fireActValInterrupt(demoValues, periodsToGenerateValuesFor);
    }
    m_lastInterruptPeriod = m_currPeriodCount;
    m_nextInterruptPeriod = m_currPeriodCount + genNextInterruptWithinPeriods();
}
