#include "demodspinterfacedspsuper.h"
#include "dspcommonsupervisor.h"
#include <timerfactoryqt.h>

DemoDspInterfaceDspSuper::DemoDspInterfaceDspSuper(int entityId,
                                                   int maxPeriodCount,
                                                   std::function<double (int)> valueGenerator) :
    m_entityId(entityId),
    m_maxPeriodCount(maxPeriodCount),
    m_valueGenerator(valueGenerator),
    m_periodicTimer(TimerFactoryQt::createPeriodic(20))
{
    connect(m_periodicTimer.get(), &TimerTemplateQt::sigExpired,
            this, &DemoDspInterfaceDspSuper::onTimer);
    connect(this, &DemoDspInterfaceDspSuper::sigDspStarted, [&](){
        m_periodicTimer->start();
    });
    for (int i=0; i<m_maxPeriodCount; ++i)
        m_demoValues.append(genValues(0.0));
}

void DemoDspInterfaceDspSuper::onTimer()
{
    float busyValFlt = m_valueGenerator(m_entityId) * 15;
    m_demoValues.append(genValues(busyValFlt));

    while(m_demoValues.size() > m_maxPeriodCount*COUNT_SUPER_ENTRIES)
        m_demoValues.removeFirst();

    m_currPeriodCount++;
    fireActValInterrupt(m_demoValues, /* dummy */ 0);
}

QVector<float> DemoDspInterfaceDspSuper::genValues(float busyVal)
{
    QVector<float> values;
    values.append(busyVal);

    float periodCountFlt = *reinterpret_cast<float*>(&m_currPeriodCount);
    values.append(periodCountFlt);

    uint msTime = 20*m_currPeriodCount;
    float msTimeFlt = *reinterpret_cast<float*>(&msTime);
    values.append(msTimeFlt);

    return values;
}
