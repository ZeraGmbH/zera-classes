#include "demodspinterfacefft.h"
#include "demovaluesdspfft.h"
#include <timerfactoryqt.h>
#include <math.h>

DemoDspInterfaceFft::DemoDspInterfaceFft(int entityId,
                                         QStringList valueChannelList,
                                         int fftOrder,
                                         std::function<double (int)> valueGenerator) :
    m_entityId(entityId),
    m_valueChannelList(valueChannelList),
    m_fftOrder(fftOrder),
    m_periodicTimer(TimerFactoryQt::createPeriodic(500)),
    m_valueGenerator(valueGenerator)
{
    connect(m_periodicTimer.get(), &TimerTemplateQt::sigExpired,
            this, &DemoDspInterfaceFft::onTimer);
    connect(this, &DemoDspInterfaceFft::sigDspStarted, [&](){
        m_periodicTimer->start();
    });
}

void DemoDspInterfaceFft::onTimer()
{
    int totalChannels = m_valueChannelList.count();
    double randomVal = m_valueGenerator(m_entityId);
    double randomDeviation = 0.95 + 0.1 * randomVal;

    DemoValuesDspFft dspValues(totalChannels);

    dspValues.setValue(0, 0, 5*randomDeviation, 0);
    dspValues.setValue(0, 1, 230*randomDeviation, 0);
    dspValues.setValue(0, 2, 0, 42*randomDeviation);

    dspValues.setValue(1, 0, -5, 0);
    dspValues.setValue(1, 1, 230, 0);
    dspValues.setValue(1, 2, 0, 42);

    dspValues.setValue(2, 0, 5, 0);
    dspValues.setValue(2, 1, 230/M_SQRT2, 230/M_SQRT2);
    dspValues.setValue(2, 2, 42/M_SQRT2, 42/M_SQRT2);

    dspValues.setValue(3, 0, 2*randomDeviation, 2*randomDeviation);
    dspValues.setValue(3, 1, 4*randomDeviation, 0);
    dspValues.setValue(3, 2, 0, 6*randomDeviation);

    dspValues.setValue(4, 0, -20, 0);
    dspValues.setValue(4, 1, -40, 0);
    dspValues.setValue(4, 2, 0, -60);

    dspValues.setValue(5, 0, 2, 2);
    dspValues.setValue(5, 1, 4/M_SQRT2, 4/M_SQRT2);
    dspValues.setValue(5, 2, 6/M_SQRT2, 6/M_SQRT2);

    if(totalChannels > 6) {
        dspValues.setValue(6, 0, 10*randomDeviation, 10*randomDeviation);
        dspValues.setValue(6, 1, 230*randomDeviation, 0);
        dspValues.setValue(6, 2, 0, 42*randomDeviation);
    }

    if(totalChannels > 7) {
        dspValues.setValue(7, 0, 20*randomDeviation, 5*randomDeviation);
        dspValues.setValue(7, 1, 230*randomDeviation, 0);
        dspValues.setValue(7, 2, 0, 42*randomDeviation);
    }

    fireActValInterrupt(dspValues.getDspValues(), /* dummy */ 0);
}
