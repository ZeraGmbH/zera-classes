#include "demodspinterfaceosci.h"
#include "demodspvaluesosci.h"
#include <timerfactoryqt.h>
#include <math.h>

DemoDspInterfaceOsci::DemoDspInterfaceOsci(int entityId,
                                           const QStringList &valueChannelList,
                                           int sampleCount,
                                           std::function<double (int)> valueGenerator) :
    m_entityId(entityId),
    m_valueChannelList(valueChannelList),
    m_sampleCount(sampleCount),
    m_periodicTimer(TimerFactoryQt::createPeriodic(500)),
    m_valueGenerator(valueGenerator)
{
    connect(m_periodicTimer.get(), &TimerTemplateQt::sigExpired,
            this, &DemoDspInterfaceOsci::onTimer);
    connect(this, &DemoDspInterfaceOsci::sigDspStarted, [&](){
        m_periodicTimer->start();
    });
}

void DemoDspInterfaceOsci::onTimer()
{
    DemoDspValuesOsci osciValues(m_entityId, m_valueChannelList, m_sampleCount, m_valueGenerator);
    osciValues.setAllValuesSymmetricAc(230, 10); // for now just AC
    QVector<float> demoValues = osciValues.getDspValues();
    fireActValInterrupt(demoValues, 6 /* that is what module expects currently */);
}
