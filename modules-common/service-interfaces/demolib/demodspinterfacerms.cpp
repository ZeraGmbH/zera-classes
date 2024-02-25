#include "demodspinterfacerms.h"
#include <timerfactoryqt.h>

DemoDspInterfaceRms::DemoDspInterfaceRms(int interruptNoHandled, QStringList valueChannelList) :
    m_interruptNoHandled(interruptNoHandled),
    m_valueChannelList(valueChannelList),
    m_periodicTimer(TimerFactoryQt::createPeriodic(500))
{
    connect(m_periodicTimer.get(), &TimerTemplateQt::sigExpired,
            this, &DemoDspInterfaceRms::onTimer);
    m_periodicTimer->start();
}

void DemoDspInterfaceRms::onTimer()
{
    QVector<float> demoValues(m_valueChannelList.count());
    for (int i = 0; i < m_valueChannelList.count(); i++) {
        QStringList sl = m_valueChannelList.at(i).split('-');
        double val = 0;
        double randPlusMinusOne = 2.0 * (double)rand() / RAND_MAX - 1.0;
        if (sl.count() == 1) {
            val = 10 * randPlusMinusOne;
        }
        else {
            val = 20 * randPlusMinusOne;
        }
        demoValues[i] = val;
    }
    fireActValInterrupt(demoValues, m_interruptNoHandled);
}
