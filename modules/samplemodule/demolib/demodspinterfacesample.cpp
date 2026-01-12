#include "demodspinterfacesample.h"
#include "demovaluesdspsample.h"
#include <timerfactoryqt.h>

DemoDspInterfaceSample::DemoDspInterfaceSample(int entityId,
                                               QStringList valueChannelList,
                                               std::function<double(int)> valueGenerator) :
    m_entityId(entityId),
    m_valueChannelList(valueChannelList),
    m_periodicTimer(TimerFactoryQt::createPeriodic(500)),
    m_valueGenerator(valueGenerator)
{
    connect(m_periodicTimer.get(), &TimerTemplateQt::sigExpired,
            this, &DemoDspInterfaceSample::onTimer);
    connect(this, &DemoDspInterfaceSample::sigDspStarted, [&](){
        m_periodicTimer->start();
    });
}

void DemoDspInterfaceSample::onTimer()
{
    DemoValuesDspSample rmsValues(m_valueChannelList);
    rmsValues.setAllValuesSymmetric(230, 1);
    QVector<float> demoValues = rmsValues.getDspValues();
    for(int i=0; i<demoValues.count(); i++) {
        double randomVal = m_valueGenerator(m_entityId);
        double randomDeviation = 0.95 + 0.1 * randomVal;
        demoValues[i] *= randomDeviation;
    }
    fireActValInterrupt(demoValues, 8 /* that is what module expects currently */);
}
