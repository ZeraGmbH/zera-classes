#include "demodspinterfacerange.h"
#include <timerfactoryqt.h>
#include <cmath>

DemoDspInterfaceRange::DemoDspInterfaceRange(int interruptNoHandled, QStringList valueChannelList, bool isReference) :
    m_interruptNoHandled(interruptNoHandled),
    m_valueChannelList(valueChannelList),
    m_isReference(isReference),
    m_periodicTimer(TimerFactoryQt::createPeriodic(500))
{
    connect(m_periodicTimer.get(), &TimerTemplateQt::sigExpired,
            this, &DemoDspInterfaceRange::onTimer);
    m_periodicTimer->start();
}

void DemoDspInterfaceRange::onTimer()
{
    QVector<float> randomChannelRMS = demoChannelRms();
    QVector<float> demoValues;
    int channelCount = m_valueChannelList.count();
    // peak
    for (int channel=0; channel<channelCount; ++channel) {
        double randPeak = randomChannelRMS[channel] * M_SQRT2;
        demoValues.append(randPeak);
    }
    // RMS
    for (int channel=0; channel<channelCount; ++channel) {
        double randRms = randomChannelRMS[channel];
        demoValues.append(randRms);
    }
    // frequency
    demoValues.append(demoFrequency());
    // peak DC (no DC for now)
    for (int channel=0; channel<channelCount; ++channel) {
        double randPeak = randomChannelRMS[channel] * M_SQRT2;
        demoValues.append(randPeak);
    }
    fireActValInterrupt(demoValues, m_interruptNoHandled);
}

bool DemoDspInterfaceRange::demoChannelIsVoltage(int channel)
{
    bool isVoltage;
    // a bit of a hack: We expect MT310s2 channel order
    switch(channel) {
    case 0:
    case 1:
    case 2:
    case 6:
        isVoltage = true;
        break;
    default:
        isVoltage = false;
        break;
    }
    return isVoltage;
}

QVector<float> DemoDspInterfaceRange::demoChannelRms()
{
    double voltageBase = m_isReference ? 10.0 : 230;
    double currentBase= 1.0;
    QVector<float> randomChannelRMS;
    int channelCount = m_valueChannelList.count();
    randomChannelRMS.resize(channelCount);
    for (int channel=0; channel<channelCount; ++channel) {
        // MT310s2 AUX I has no clamp in demo - this has room for enhancement
        if(m_valueChannelList[channel] != "m7") {
            bool isVoltage = demoChannelIsVoltage(channel);
            double baseRMS = isVoltage ? voltageBase : currentBase;
            double randPlusMinusOne = 2.0 * (double)rand() / RAND_MAX - 1.0;
            double randOffset = 0.02 * randPlusMinusOne;
            double randRMS = (1+randOffset) * baseRMS;
            randomChannelRMS[channel] = randRMS;
        }
        else
            randomChannelRMS[channel] = 0;
    }
    return randomChannelRMS;
}

double DemoDspInterfaceRange::demoFrequency()
{
    double freqBase= 50.0;
    double randPlusMinusOne = 2.0 * (double)rand() / RAND_MAX - 1.0;
    double randOffset = 0.02 * randPlusMinusOne;
    double randRMS = (1+randOffset) * freqBase;
    return randRMS;
}