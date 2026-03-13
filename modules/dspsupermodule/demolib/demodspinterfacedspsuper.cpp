#include "demodspinterfacedspsuper.h"
#include "demovaluesdspdspsuper.h"
#include <timerfactoryqt.h>

DemoDspInterfaceDspSuper::DemoDspInterfaceDspSuper(int entityId,
                                                   int maxPeriodCount,
                                                   std::function<double (int)> valueGenerator) :
    m_entityId(entityId),
    m_maxPeriodCount(maxPeriodCount),
    m_valueGenerator(valueGenerator),
    m_periodicTimer(TimerFactoryQt::createPeriodic(500))
{
    connect(m_periodicTimer.get(), &TimerTemplateQt::sigExpired,
            this, &DemoDspInterfaceDspSuper::onTimer);
    connect(this, &DemoDspInterfaceDspSuper::sigDspStarted, [&](){
        m_periodicTimer->start();
    });
}

void DemoDspInterfaceDspSuper::onTimer()
{
    /*DemoValuesDspDspSuper values(m_valueChannelList, m_maxPeriodCount, m_periodCount);
    QVector<float> demoValues;
    for (int channelNo=0; channelNo<m_valueChannelList.count(); channelNo++)
        values.setValue(m_valueChannelList[channelNo], channelNo, m_valueGenerator(channelNo));
    demoValues = values.getDspValues();
    fireActValInterrupt(demoValues,  0);*/
}
