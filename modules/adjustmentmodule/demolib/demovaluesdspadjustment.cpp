#include "demovaluesdspadjustment.h"
#include "servicechannelnamehelper.h"

DemoValuesDspAdjustment::DemoValuesDspAdjustment(QStringList valueChannelList)
{
    for(const QString& valueChannel : valueChannelList) {
        if(valueChannel.split("-").count() == 1)
            m_channelList.append(valueChannel);
    }
    m_dftValues = std::make_unique<DemoValuesDspDft>(valueChannelList, 1);
    m_fftValues = std::make_unique<DemoValuesDspFft>(m_channelList.count());
    m_rangeValues = std::make_unique<DemoValuesDspRange>(m_channelList.count());
    m_rmsValues = std::make_unique<DemoValuesDspRms>(valueChannelList);
}

void DemoValuesDspAdjustment::setAllValuesSymmetric(float voltage, float current, float angleUi, float frequency, bool invertedSequence)
{
    m_dftValues->setAllValuesSymmetric(voltage, current, angleUi, invertedSequence);
    for(int channel=0; channel<m_channelList.count(); channel++) {
        float value = ServiceChannelNameHelper::isCurrent(m_channelList[channel]) ? current : voltage;
        m_fftValues->setValue(channel, 0, value, 0);
        m_rangeValues->setRmsValue(channel, value);
        m_rangeValues->setFrequency(frequency);
        m_rmsValues->setAllValuesSymmetric(voltage, current);
    }
}

void DemoValuesDspAdjustment::fireActualValues(MockDspInterface *dspDft, MockDspInterface *dspFft, MockDspInterface *dspRange, MockDspInterface *dspRms)
{
    //    dspDft->fireActValInterrupt();
}

QVector<float> DemoValuesDspAdjustment::getDspValuesDft()
{
    return m_dftValues->getDspValues();
}

QVector<float> DemoValuesDspAdjustment::getDspValuesFft()
{
    return m_dftValues->getDspValues();
}

QVector<float> DemoValuesDspAdjustment::getDspValuesRange()
{
    return m_rangeValues->getDspValues();
}

QVector<float> DemoValuesDspAdjustment::getDspValuesRms()
{
    return m_rmsValues->getDspValues();
}
