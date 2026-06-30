#include "demodspvaluesosci.h"
#include "servicechannelnamehelper.h"
#include <math.h>

DemoDspValuesOsci::DemoDspValuesOsci(const QStringList &valueChannelList) :
    m_valueChannelList(valueChannelList),
    m_valueGenerator([](int) { return 1.0; })
{
}

DemoDspValuesOsci::DemoDspValuesOsci(int entityId,
                                     const QStringList &valueChannelList,
                                     int sampleCount,
                                     std::function<double (int)> valueGenerator) :
    m_entityId(entityId),
    m_valueChannelList(valueChannelList),
    m_sampleCount(sampleCount),
    m_valueGenerator(valueGenerator)
{
}

void DemoDspValuesOsci::setAllValuesSymmetricAc(float voltage, float current)
{
    QVector<float> demoValues;
    for(const QString &channelName : qAsConst(m_valueChannelList)) {
        double randomVal = m_valueGenerator(m_entityId);
        float phase = 0.0;
        if(channelName == "m1" || channelName == "m4") //UL2,IL2
            phase = randomVal * M_PI/2;
        else if(channelName == "m2" || channelName == "m5") //UL3,IL3
            phase = randomVal * -M_PI/2;
        else
            phase = 0.0;

        if(ServiceChannelNameHelper::isCurrent(channelName))
            demoValues.append(generateSineCurve(m_sampleCount, current, phase));
        else
            demoValues.append(generateSineCurve(m_sampleCount, voltage, phase));
    }
    m_dspValues = demoValues;
}

void DemoDspValuesOsci::setAllValuesSymmetricDc(float voltage, float current)
{
    QVector<float> demoValues;
    for(const QString &channelName : qAsConst(m_valueChannelList)) {
        double randomVal = m_valueGenerator(m_entityId);
        if(ServiceChannelNameHelper::isCurrent(channelName))
            demoValues.append(generateDcCurve(m_sampleCount, current * randomVal));
        else
            demoValues.append(generateDcCurve(m_sampleCount, voltage * randomVal));
    }
    m_dspValues = demoValues;
}

QVector<float> DemoDspValuesOsci::getDspValues() const
{
    return m_dspValues;
}

QVector<float> DemoDspValuesOsci::generateSineCurve(int sampleCount, int amplitude, float phase)
{
    QVector<float> samples(sampleCount, 0.0);
    for(int i = 0; i < sampleCount; i++)
        samples[i] = amplitude * M_SQRT2 * sin((2 * M_PI * i / sampleCount) + phase);
    return samples;
}

QVector<float> DemoDspValuesOsci::generateDcCurve(int sampleCount, int amplitude)
{
    QVector<float> samples(sampleCount, 0.0);
    for(int i = 0; i < sampleCount; i++)
        samples[i] = amplitude;
    return samples;
}
