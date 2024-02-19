#include "demodspinterfacefft.h"
#include <math.h>

DemoDspInterfaceFft::DemoDspInterfaceFft(int interruptNoHandled, QStringList valueChannelList, int fftOrder) :
    m_interruptNoHandled(interruptNoHandled),
    m_valueChannelList(valueChannelList),
    m_fftOrder(fftOrder),
    m_periodicTimer(500)
{
    connect(&m_periodicTimer, &TimerPeriodicQt::sigExpired,
            this, &DemoDspInterfaceFft::onTimer);
    m_periodicTimer.start();
}

void DemoDspInterfaceFft::onTimer()
{
    int totalChannels = m_valueChannelList.count();

    // currently we caused a regression: Old demo created displayable result.
    // Now the job is to create data before cFftModuleMeasProgram::dataReadDSP()
    // adjusts it. Yepp same data btw...
    // check cFftModuleMeasProgram::setDspVarList() and cFftModuleMeasProgram::dataReadDSP() for more details
    int fftLen = 2 << (int)(floor(log((m_fftOrder << 1) -1)/log(2.0))); // our fft length
    QVector<float> demoValues(totalChannels * fftLen * 2 * totalChannels, 0.0);

    for (int i = 0; i < totalChannels; i++) {
        double randomVal = (double)rand() / RAND_MAX ;
        // just real part
        int channelOffset = i * m_fftOrder * 2;
        QStringList voltageChannelList = QStringList() << "m0" << "m1" << "m2" << "m6";
        // DC
        if(i == 0)
            demoValues.insert(channelOffset, randomVal*0.5);
        else {
            float mult = voltageChannelList.contains(m_valueChannelList[i]) ? 230 : 10;
            demoValues.insert(channelOffset, randomVal * mult); //fundamental frequency component, real part
        }
    }

    fireActValInterrupt(demoValues, m_interruptNoHandled);
}
