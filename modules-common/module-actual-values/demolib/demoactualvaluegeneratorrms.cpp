#include "demoactualvaluegeneratorrms.h"
#include <timerfactoryqt.h>

DemoActualValueGeneratorRms::DemoActualValueGeneratorRms(QStringList valueChannelList) :
    m_valueChannelList(valueChannelList),
    m_periodicTimerActGen(TimerFactoryQt::createPeriodic(500))
{
    connect(m_periodicTimerActGen.get(), &TimerTemplateQt::sigExpired,
            this, &DemoActualValueGeneratorRms::onNewActValues);
}

void DemoActualValueGeneratorRms::start()
{
    m_periodicTimerActGen->start();
}

void DemoActualValueGeneratorRms::stop()
{
    m_periodicTimerActGen->stop();
}

void DemoActualValueGeneratorRms::onNewActValues()
{
    QVector<float> demoValues;
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
        demoValues.append(val);
    }
    emit sigNewActualValues(demoValues);
}

