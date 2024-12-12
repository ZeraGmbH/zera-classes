#include "demodspinterfacethdn.h"
#include <timerfactoryqt.h>
#include <math.h>

DemoDspInterfaceThdn::DemoDspInterfaceThdn(QStringList valueChannelList) :
    m_valueChannelList(valueChannelList),
    m_periodicTimer(TimerFactoryQt::createPeriodic(500))
{
    connect(m_periodicTimer.get(), &TimerTemplateQt::sigExpired,
            this, &DemoDspInterfaceThdn::onTimer);
    connect(this, &DemoDspInterfaceThdn::sigDspStarted, [&](){
        m_periodicTimer->start();
    });
}

void DemoDspInterfaceThdn::onTimer()
{
    int valueCount = m_valueChannelList.count();
    QVector<float> demoValues(valueCount);
    // This has room for enhancements...
    for(int i=0; i<valueCount; ++i) {
        double randomVal = (double)rand() / float(RAND_MAX);
        double randomDeviation = 0.95 + 0.1 * randomVal;
        float val = (10+i) * randomDeviation;
        float thdr = val / sqrt(1 + (val * val));
        demoValues[i] = thdr;
    }
    fireActValInterrupt(demoValues, 4 /* that is what module expects currently */);
}
