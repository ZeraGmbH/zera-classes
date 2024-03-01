#include "demodspinterfacefft.h"
#include "demovaluesdspfft.h"
#include <timerfactoryqt.h>
#include <math.h>

DemoDspInterfaceFft::DemoDspInterfaceFft(int interruptNoHandled, QStringList valueChannelList, int fftOrder) :
    m_interruptNoHandled(interruptNoHandled),
    m_valueChannelList(valueChannelList),
    m_fftOrder(fftOrder),
    m_periodicTimer(TimerFactoryQt::createPeriodic(500))
{
    connect(m_periodicTimer.get(), &TimerTemplateQt::sigExpired,
            this, &DemoDspInterfaceFft::onTimer);
    m_periodicTimer->start();
}

void DemoDspInterfaceFft::onTimer()
{
    int totalChannels = m_valueChannelList.count();
    double randomVal = (double)rand() / RAND_MAX;
    double randomDeviation = 0.95 + 0.1 * randomVal;

    DemoValuesDspFft dspValues(totalChannels);

    dspValues.setValue(0, 0, 5*randomDeviation, 5*randomDeviation);
    dspValues.setValue(0, 1, 230*randomDeviation, 0);
    dspValues.setValue(0, 2, 0, 42*randomDeviation);

    dspValues.setValue(1, 0, 5, 5);
    dspValues.setValue(1, 1, 230, 0);
    dspValues.setValue(1, 2, 0, 42);

    dspValues.setValue(2, 0, 5, 5);
    dspValues.setValue(2, 1, 230/M_SQRT2, 230/M_SQRT2);
    dspValues.setValue(2, 2, 42/M_SQRT2, 42/M_SQRT2);

    dspValues.setValue(3, 0, 2*randomDeviation, 2*randomDeviation);
    dspValues.setValue(3, 1, 4*randomDeviation, 0);
    dspValues.setValue(3, 2, 0, 6*randomDeviation);

    dspValues.setValue(4, 0, 2, 2);
    dspValues.setValue(4, 1, 4, 0);
    dspValues.setValue(4, 2, 0, 6);

    dspValues.setValue(5, 0, 2, 2);
    dspValues.setValue(5, 1, 4/M_SQRT2, 4/M_SQRT2);
    dspValues.setValue(5, 2, 6/M_SQRT2, 6/M_SQRT2);

    fireActValInterrupt(dspValues.getDspValues(), m_interruptNoHandled);
}
