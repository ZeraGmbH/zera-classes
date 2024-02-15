#include "demoactvalmaninthemiddledft.h"
#include <complex>
#include <timerfactoryqt.h>
#include <QHash>

DemoActValManInTheMiddleDft::DemoActValManInTheMiddleDft(QStringList valueChannelList) :
    m_valueChannelList(valueChannelList),
    m_periodicTimerActGen(TimerFactoryQt::createPeriodic(500)),
    m_demoValues(2 * m_valueChannelList.count())
{
    connect(m_periodicTimerActGen.get(), &TimerTemplateQt::sigExpired,
            this, &DemoActValManInTheMiddleDft::onNewActValues);
}

void DemoActValManInTheMiddleDft::start()
{
    m_periodicTimerActGen->start();
}

void DemoActValManInTheMiddleDft::stop()
{
    m_periodicTimerActGen->stop();
}

void DemoActValManInTheMiddleDft::onNewActValues()
{

    for (int i = 0; i < m_demoValues.count(); i++) {
        double randomVal = (double)rand() / RAND_MAX ;
        m_demoValues[i] = randomVal;
    }

    // this part is copied from dataReadDsp for better simulated values
    double im;
    for (int i = 0; i < m_valueChannelList.count(); i++) {
        im = m_demoValues[i*2+1] * -1.0;
        m_demoValues.replace(i*2+1, im);
    }
    emit sigNewActualValues(&m_demoValues);
}
