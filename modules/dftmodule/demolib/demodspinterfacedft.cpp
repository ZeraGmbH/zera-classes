#include "demodspinterfacedft.h"
#include "demovaluesdspdft.h"
#include <timerfactoryqt.h>

DemoDspInterfaceDft::DemoDspInterfaceDft(QStringList valueChannelList, int dftOrder) :
    m_valueChannelList(valueChannelList),
    m_dftOrder(dftOrder),
    m_periodicTimer(TimerFactoryQt::createPeriodic(500))
{
    connect(m_periodicTimer.get(), &TimerTemplateQt::sigExpired,
            this, &DemoDspInterfaceDft::onTimer);
    connect(this, &DemoDspInterfaceDft::sigDspStarted, [&](){
        m_periodicTimer->start();
    });
}

void DemoDspInterfaceDft::onTimer()
{
    DemoValuesDspDft dftValues(m_valueChannelList, m_dftOrder);
    QVector<float> demoValues;
    if(m_dftOrder > 0) {
        m_currentAngle += 7.5;
        if(m_currentAngle > 359)
            m_currentAngle = 0;
        dftValues.setAllValuesSymmetric(230, 1, m_currentAngle, false);
        demoValues = dftValues.getDspValues();
        for(int i=0; i<demoValues.count(); i++) {
            float randomVal = (double)rand() / RAND_MAX;
            float randomDeviation = 0.5 + randomVal;
            //demoValues[i] *= randomDeviation;
        }
    }
    else {
        dftValues.setAllValuesSymmetric(10, 10, 0, false); // COM DC ref
        demoValues = dftValues.getDspValues();
        for(int i=0; i<demoValues.count(); i++) {
            float randomVal = (double)rand() / RAND_MAX;
            float randomDeviation = 0.95 + 0.1*randomVal;
            //demoValues[i] *= randomDeviation;
        }
    }
    fireActValInterrupt(demoValues, /* dummy */ 0);
}
