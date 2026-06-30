#include "demodspinterfacethdn.h"
#include "demodspvaluesthd.h"
#include <timerfactoryqt.h>
#include <math.h>

DemoDspInterfaceThdn::DemoDspInterfaceThdn(int entityId, const QStringList &valueChannelList,
                                           std::function<double (int)> valueGenerator) :
    m_entityId(entityId),
    m_valueChannelList(valueChannelList),
    m_periodicTimer(TimerFactoryQt::createPeriodic(500)),
    m_valueGenerator(valueGenerator)
{
    connect(m_periodicTimer.get(), &TimerTemplateQt::sigExpired,
            this, &DemoDspInterfaceThdn::onTimer);
    connect(this, &DemoDspInterfaceThdn::sigDspStarted, [&](){
        m_periodicTimer->start();
    });
}

void DemoDspInterfaceThdn::onTimer()
{
    DemoDspValuesThd thdnValues(m_entityId, m_valueChannelList, m_valueGenerator);
    thdnValues.setAllValuesSymmetric();
    fireActValInterrupt(thdnValues.getDspValues(), 4 /* that is what module expects currently */);
}
