#include "demoactvalmaninthemiddlerms.h"
#include <timerfactoryqt.h>

DemoActValManInTheMiddleRms::DemoActValManInTheMiddleRms(QStringList valueChannelList) :
    m_valueChannelList(valueChannelList),
    m_periodicTimerActGen(TimerFactoryQt::createPeriodic(500)),
    m_demoValues(m_valueChannelList.count())
{
    connect(m_periodicTimerActGen.get(), &TimerTemplateQt::sigExpired,
            this, &DemoActValManInTheMiddleRms::onNewActValues);
}

void DemoActValManInTheMiddleRms::start()
{
    m_periodicTimerActGen->start();
}

void DemoActValManInTheMiddleRms::stop()
{
    m_periodicTimerActGen->stop();
}

void DemoActValManInTheMiddleRms::onNewActValues()
{
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
        m_demoValues[i] = val;
    }
    emit sigNewActualValues(&m_demoValues);
}

