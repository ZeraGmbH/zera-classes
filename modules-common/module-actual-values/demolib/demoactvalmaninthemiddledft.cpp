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

    /*QHash<QString, std::complex<double>> DftActValuesHash;
    if (getConfData()->m_bRefChannelOn) {
        for (int i = 0; i < getConfData()->m_valueChannelList.count(); i++)
            DftActValuesHash[getConfData()->m_valueChannelList.at(i)] = std::complex<double>(m_ModuleActualValues[i*2], m_ModuleActualValues[i*2+1]);

        for (int i = 0; i < getConfData()->m_valueChannelList.count(); i++) {
            QString key;
            QStringList sl;
            key = getConfData()->m_valueChannelList.at(i);
            sl = key.split('-');
            // we have 2 entries
            if (sl.count() == 2) {
                DftActValuesHash.remove(key);
                DftActValuesHash[key] = DftActValuesHash[sl.at(0)] - DftActValuesHash[sl.at(1)];
            }
            m_ModuleActualValues.replace(i*2, DftActValuesHash[key].real());
            m_ModuleActualValues.replace(i*2+1, DftActValuesHash[key].imag());
        }
    }*/
}
