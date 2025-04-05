#include "demodspinterfacedft.h"
#include "demovaluesdspdft.h"
#include <timerfactoryqt.h>

DemoDspInterfaceDft::DemoDspInterfaceDft(QStringList valueChannelList,
                                         int dftOrder,
                                         ValueTypes valueType) :
    m_valueChannelList(valueChannelList),
    m_dftOrder(dftOrder),
    m_periodicTimer(TimerFactoryQt::createPeriodic(500)),
    m_valueType(valueType)
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
        if (m_valueType == RotatingValues) {
            m_currentAngle += 7.5;
            if(m_currentAngle > 359)
                m_currentAngle = 0;
        }
        dftValues.setAllValuesSymmetric(230, 1, m_currentAngle, false);
        demoValues = dftValues.getDspValues();
    }
    else {
        dftValues.setAllValuesSymmetric(10, 10, 0, false); // COM DC ref
        demoValues = dftValues.getDspValues();
    }
    fireActValInterrupt(demoValues, /* dummy */ 0);
}
