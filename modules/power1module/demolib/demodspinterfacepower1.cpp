#include "demodspinterfacepower1.h"
#include "demovaluesdsppower1.h"
#include <timerfactoryqt.h>

DemoDspInterfacePower1::DemoDspInterfacePower1(int entityId,
                                               MeasModeSelector *measMode,
                                               std::function<double (int)> valueGenerator) :
    m_entityId(entityId),
    m_measMode(measMode),
    m_periodicTimer(TimerFactoryQt::createPeriodic(500)),
    m_valueGenerator(valueGenerator)
{
    connect(m_periodicTimer.get(), &TimerTemplateQt::sigExpired,
            this, &DemoDspInterfacePower1::onTimer);
    connect(this, &DemoDspInterfacePower1::sigDspStarted, [&](){
        m_periodicTimer->start();
    });
}

void DemoDspInterfacePower1::onTimer()
{
    DemoValuesDspPower1 power1Values(m_entityId, m_valueGenerator, m_measMode);
    power1Values.setAllValuesSymmetric(230, 10);
    QVector<float> valuesDemo = power1Values.getDspValues();
    fireActValInterrupt(valuesDemo,  /* dummy */ 0);
}
