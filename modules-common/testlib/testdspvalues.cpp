#include "testdspvalues.h"
#include "servicechannelnamehelper.h"

TestDspValues::TestDspValues(QStringList valueChannelList)
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

void TestDspValues::setAllValuesSymmetric(float voltage, float current, float angleUi, float frequency, bool invertedSequence)
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

void TestDspValues::fireActualValues(MockDspInterfacePtr dspDft,
                                               MockDspInterfacePtr dspFft,
                                               MockDspInterfacePtr dspRange,
                                               MockDspInterfacePtr dspRms)
{
    dspDft->fireActValInterrupt(m_dftValues->getDspValues(), 0 /* dummy */);
    dspFft->fireActValInterrupt(m_fftValues->getDspValues(), 0 /* dummy */);
    dspRange->fireActValInterrupt(m_rangeValues->getDspValues(), 0 /* dummy */);
    dspRms->fireActValInterrupt(m_rmsValues->getDspValues(), 0 /* dummy */);
}

QVector<float> TestDspValues::getDspValuesDft()
{
    return m_dftValues->getDspValues();
}

QVector<float> TestDspValues::getDspValuesFft()
{
    return m_dftValues->getDspValues();
}

QVector<float> TestDspValues::getDspValuesRange()
{
    return m_rangeValues->getDspValues();
}

QVector<float> TestDspValues::getDspValuesRms()
{
    return m_rmsValues->getDspValues();
}
