#include "demodspinterfacedft.h"

DemoDspInterfaceDft::DemoDspInterfaceDft(int interruptNoHandled, QStringList valueChannelList) :
    m_interruptNoHandled(interruptNoHandled),
    m_valueChannelList(valueChannelList),
    m_periodicTimer(500)
{
    connect(&m_periodicTimer, &TimerPeriodicQt::sigExpired,
            this, &DemoDspInterfaceDft::onTimer);
    m_periodicTimer.start();
}

void DemoDspInterfaceDft::onTimer()
{
    QVector<float> demoValues(m_valueChannelList.count() * 2); // re/im
    for (int i = 0; i < demoValues.count(); i++) {
        double randomVal = (double)rand() / RAND_MAX ;
        demoValues[i] = randomVal;
    }

    // this part is copied from dataReadDsp for better simulated values
    double im;
    for (int i = 0; i < m_valueChannelList.count(); i++) {
        im = demoValues[i*2+1] * -1.0;
        demoValues.replace(i*2+1, im);
    }
    fireActValInterrupt(demoValues, m_interruptNoHandled);
}
