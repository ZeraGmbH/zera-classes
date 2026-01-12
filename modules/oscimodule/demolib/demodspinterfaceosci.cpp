#include "demodspinterfaceosci.h"
#include "servicechannelnamehelper.h"
#include <timerfactoryqt.h>
#include <math.h>

DemoDspInterfaceOsci::DemoDspInterfaceOsci(int entityId,
                                           QStringList valueChannelList,
                                           int interpolation,
                                           std::function<double (int)> valueGenerator) :
    m_entityId(entityId),
    m_valueChannelList(valueChannelList),
    m_interpolation(interpolation),
    m_periodicTimer(TimerFactoryQt::createPeriodic(500)),
    m_valueGenerator(valueGenerator)
{
    connect(m_periodicTimer.get(), &TimerTemplateQt::sigExpired,
            this, &DemoDspInterfaceOsci::onTimer);
    connect(this, &DemoDspInterfaceOsci::sigDspStarted, [&](){
        m_periodicTimer->start();
    });
}

QVector<float> DemoDspInterfaceOsci::generateSineCurve(int sampleCount, int amplitude, float phase)
{
    QVector<float> samples;
    for(int i = 0; i < sampleCount; i++) {
        samples.append(amplitude * M_SQRT2 * sin((2 * M_PI * i / sampleCount) + phase));
    }
    return samples;
}

void DemoDspInterfaceOsci::onTimer()
{
    QVector<float> demoValues;
    float phase = 0.0;
    for(const QString &channelName : qAsConst(m_valueChannelList)) {
        double randomVal = m_valueGenerator(m_entityId);
        if(channelName == "m1" || channelName == "m4") //UL2,IL2
            phase = randomVal * M_PI/2;
        else if(channelName == "m2" || channelName == "m5") //UL3,IL3
            phase = randomVal * -M_PI/2;
        else
            phase = 0.0;

        if(ServiceChannelNameHelper::isCurrent(channelName))
            demoValues.append(generateSineCurve(m_interpolation, 10, phase));
        else
            demoValues.append(generateSineCurve(m_interpolation, 230, phase));
    }
    fireActValInterrupt(demoValues, 6 /* that is what module expects currently */);
}
