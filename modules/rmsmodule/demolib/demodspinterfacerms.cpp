#include "demodspinterfacerms.h"
#include "demovaluesdsprms.h"
#include <timerfactoryqt.h>

DemoDspInterfaceRms::DemoDspInterfaceRms(int interruptNoHandled, QStringList valueChannelList) :
    m_interruptNoHandled(interruptNoHandled),
    m_valueChannelList(valueChannelList),
    m_periodicTimer(TimerFactoryQt::createPeriodic(500))
{
    connect(m_periodicTimer.get(), &TimerTemplateQt::sigExpired,
            this, &DemoDspInterfaceRms::onTimer);
    connect(this, &DemoDspInterfaceRms::sigDspStarted, [&](){
        m_periodicTimer->start();
    });
}

void DemoDspInterfaceRms::onTimer()
{
    DemoValuesDspRms rmsValues(m_valueChannelList);
    rmsValues.setAllValuesSymmetric(230, 1);
    QVector<float> demoValues = rmsValues.getDspValues();
    for(int i=0; i<demoValues.count(); i++) {
        double randomVal = (double)rand() / RAND_MAX;
        double randomDeviation = 0.95 + 0.1 * randomVal;
        demoValues[i] *= randomDeviation;
    }
    fireActValInterrupt(demoValues, m_interruptNoHandled);
}
