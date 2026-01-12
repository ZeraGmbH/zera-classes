#include "demodspinterfacerange.h"
#include "demovaluesdsprange.h"
#include <timerfactoryqt.h>

DemoDspInterfaceRange::DemoDspInterfaceRange(int entityId,
                                             QStringList valueChannelList,
                                             bool isReference,
                                             std::function<double (int)> valueGenerator) :
    m_entityId(entityId),
    m_valueChannelList(valueChannelList),
    m_isReference(isReference),
    m_periodicTimer(TimerFactoryQt::createPeriodic(500)),
    m_valueGenerator(valueGenerator)
{
    connect(m_periodicTimer.get(), &TimerTemplateQt::sigExpired,
            this, &DemoDspInterfaceRange::onTimer);
    connect(this, &DemoDspInterfaceRange::sigDspStarted, [&](){
        m_periodicTimer->start();
    });
}

void DemoDspInterfaceRange::onTimer()
{
    QVector<float> randomChannelRMS = demoChannelRms();
    DemoValuesDspRange rangeValues(m_valueChannelList.count());
    for(int channel=0; channel<m_valueChannelList.count(); channel++)
        rangeValues.setRmsPeakDCValue(channel, randomChannelRMS[channel]);
    rangeValues.setFrequency(demoFrequency());
    fireActValInterrupt(rangeValues.getDspValues(), 0 /* dummy */);
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
            double randPlusMinusOne = 2.0 * m_valueGenerator(m_entityId) - 1.0;
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
    double randPlusMinusOne = 1.0 * m_valueGenerator(m_entityId) - 0.5;
    double randOffset = 0.02 * randPlusMinusOne;
    double randRMS = (1+randOffset) * freqBase;
    return randRMS;
}
