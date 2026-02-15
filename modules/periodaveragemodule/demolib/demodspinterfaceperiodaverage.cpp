#include "demodspinterfaceperiodaverage.h"
#include "demovaluesdspperiodaverage.h"
#include <timerfactoryqt.h>

DemoDspInterfacePeriodAverage::DemoDspInterfacePeriodAverage(int entityId,
                                                             QStringList valueChannelList,
                                                             std::function<double (int)> valueGenerator) :
    m_entityId(entityId),
    m_valueChannelList(valueChannelList),
    m_valueGenerator(valueGenerator),
    m_periodicTimer(TimerFactoryQt::createPeriodic(500))
{
    connect(m_periodicTimer.get(), &TimerTemplateQt::sigExpired,
            this, &DemoDspInterfacePeriodAverage::onTimer);
    connect(this, &DemoDspInterfacePeriodAverage::sigDspStarted, [&](){
        m_periodicTimer->start();
    });
}

void DemoDspInterfacePeriodAverage::onTimer()
{
    DemoValuesDspPeriodAverage values(m_valueChannelList);
    QVector<float> demoValues;
    for (int channelNo=0; channelNo<m_valueChannelList.count(); channelNo++)
        values.setValue(m_valueChannelList[channelNo], channelNo, m_valueGenerator(channelNo));
    demoValues = values.getDspValues();
    fireActValInterrupt(demoValues, /* dummy */ 0);
}
