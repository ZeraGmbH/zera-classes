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
        m_rangeValues->setRmsPeakDCValue(channel, value);
        m_rangeValues->setFrequency(frequency);
        m_rmsValues->setAllValuesSymmetric(voltage, current);
    }
}

void TestDspValues::setAllValuesSymmetricAc(float voltage, float current, float angleUi, float frequency, bool invertedSequence)
{
    m_dftValues->setAllValuesSymmetric(voltage, current, angleUi, invertedSequence);
    for(int channel=0; channel<m_channelList.count(); channel++) {
        float value = ServiceChannelNameHelper::isCurrent(m_channelList[channel]) ? current : voltage;
        m_fftValues->setValue(channel, 0, 0, 0); // DC
        m_rangeValues->setRmsPeakDCValue(channel, value);
        m_rangeValues->setFrequency(frequency);
        m_rmsValues->setAllValuesSymmetric(voltage, current);
    }
}

void TestDspValues::setAllValuesSymmetricDc(float voltage, float current)
{
    m_dftValues->setAllValuesSymmetric(0, 0, 0, false);
    for(int channel=0; channel<m_channelList.count(); channel++) {
        float value = ServiceChannelNameHelper::isCurrent(m_channelList[channel]) ? current : voltage;
        m_fftValues->setValue(channel, 0, value, 0); // DC
        m_rangeValues->setRmsPeakDCValue(channel, value);
        m_rangeValues->setFrequency(0);
        m_rmsValues->setAllValuesSymmetric(0, 0);
    }
}

void TestDspValues::fireDftActualValues(MockDspInterfacePtr dspDft)
{
    dspDft->fireActValInterrupt(m_dftValues->getDspValues(), 0 /* dummy */);
}

void TestDspValues::fireFftActualValues(MockDspInterfacePtr dspFft)
{
    dspFft->fireActValInterrupt(m_fftValues->getDspValues(), 0 /* dummy */);
}

void TestDspValues::fireRangeActualValues(MockDspInterfacePtr dspRange)
{
    dspRange->fireActValInterrupt(m_rangeValues->getDspValues(), 0 /* dummy */);
}

void TestDspValues::fireRmsActualValues(MockDspInterfacePtr dspRms)
{
    dspRms->fireActValInterrupt(m_rmsValues->getDspValues(), 0 /* dummy */);
}

void TestDspValues::fireAllActualValues(MockDspInterfacePtr dspDft,
                                               MockDspInterfacePtr dspFft,
                                               MockDspInterfacePtr dspRange,
                                               MockDspInterfacePtr dspRms)
{
    fireDftActualValues(dspDft);
    fireFftActualValues(dspFft);
    fireRangeActualValues(dspRange);
    fireRmsActualValues(dspRms);
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
