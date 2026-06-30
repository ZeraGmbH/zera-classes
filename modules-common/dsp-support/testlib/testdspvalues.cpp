#include "testdspvalues.h"
#include "servicechannelnamehelper.h"

TestDspValues::TestDspValues(const QStringList &valueChannelList, int dftOrder)
{
    for(const QString& valueChannel : valueChannelList) {
        if(valueChannel.split("-").count() == 1)
            m_channelList.append(valueChannel);
    }
    m_dftValues = std::make_unique<DemoDspValuesDft>(valueChannelList, dftOrder);
    m_fftValues = std::make_unique<DemoDspValuesFft>(m_channelList.count());
    m_rangeValues = std::make_unique<DemoDspValuesRange>(m_channelList.count());
    m_rmsValues = std::make_unique<DemoDspValuesRms>(valueChannelList);
    m_power1Values = std::make_unique<DemoDspValuesPower1>();
    m_osciValues = std::make_unique<DemoDspValuesOsci>(valueChannelList);
    m_thdValues = std::make_unique<DemoDspValuesThd>(valueChannelList);
}

void TestDspValues::setAllValuesSymmetricAc(float voltage, float current, float angleUi, float frequency, bool invertedSequence)
{
    m_dftValues->setAllValuesSymmetric(voltage, current, angleUi, invertedSequence);
    for(int channel=0; channel<m_channelList.count(); channel++) {
        float value = ServiceChannelNameHelper::isCurrent(m_channelList[channel]) ? current : voltage;
        for (int harmonic = 1; harmonic < m_fftValues->getFFtLen(); ++harmonic)
            m_fftValues->setValue(channel, harmonic, value/harmonic, 0);
        m_rangeValues->setRmsPeakDCValue(channel, value);
        m_rangeValues->setFrequency(frequency);
        m_rmsValues->setAllValuesSymmetric(voltage, current);
        m_power1Values->setAllValuesSymmetric(voltage, current);
        m_osciValues->setAllValuesSymmetricAc(voltage, current);
        m_thdValues->setAllValuesSymmetric();
    }
}

void TestDspValues::setAllValuesSymmetricDc(float voltage, float current)
{
    m_dftValues->setAllValuesSymmetricDc(voltage, current);
    for(int channel=0; channel<m_channelList.count(); channel++) {
        float value = ServiceChannelNameHelper::isCurrent(m_channelList[channel]) ? current : voltage;
        m_fftValues->setValue(channel, 0, value, 0); // DC
        m_rangeValues->setRmsPeakDCValue(channel, value);
        m_rangeValues->setFrequency(0);
        m_rmsValues->setAllValuesSymmetric(0, 0);
        m_power1Values->setAllValuesSymmetric(voltage, current);
        m_osciValues->setAllValuesSymmetricDc(voltage, current);
        m_thdValues->setAllValuesSymmetric();
    }
}

void TestDspValues::fireDftActualValues(const MockDspInterfacePtr &dspDft)
{
    if (dspDft)
        dspDft->fireActValInterrupt(m_dftValues->getDspValues(), 0 /* dummy */);
}

void TestDspValues::fireFftActualValues(const MockDspInterfacePtr &dspFft)
{
    if (dspFft)
        dspFft->fireActValInterrupt(m_fftValues->getDspValues(), 0 /* dummy */);
}

void TestDspValues::fireRangeActualValues(const MockDspInterfacePtr &dspRange)
{
    if (dspRange)
        dspRange->fireActValInterrupt(m_rangeValues->getDspValues(), 0 /* dummy */);
}

void TestDspValues::fireRmsActualValues(const MockDspInterfacePtr &dspRms)
{
    if (dspRms)
        dspRms->fireActValInterrupt(m_rmsValues->getDspValues(), 0 /* dummy */);
}

void TestDspValues::firePower1ActualValues(const QList<TestDspInterfacePtr> &dspPower1)
{
    for(const MockDspInterfacePtr &dspInterface : dspPower1)
        dspInterface->fireActValInterrupt(m_power1Values->getDspValues(), 0 /* dummy */);
}

void TestDspValues::fireOsciActualValues(const MockDspInterfacePtr &dspOsci)
{
    if (dspOsci)
        dspOsci->fireActValInterrupt(m_osciValues->getDspValues(), 0 /* dummy */);
}

void TestDspValues::fireThdActualValues(const QList<TestDspInterfacePtr> &dspThd)
{
    for(const MockDspInterfacePtr &dspInterface : dspThd)
        dspInterface->fireActValInterrupt(m_thdValues->getDspValues(), 0 /* dummy */);
}

void TestDspValues::fireAllActualValues(const MockDspInterfacePtr &dspDft,
                                        const MockDspInterfacePtr &dspFft,
                                        const MockDspInterfacePtr &dspRange,
                                        const MockDspInterfacePtr &dspRms,
                                        const QList<TestDspInterfacePtr> &dspPower1,
                                        const MockDspInterfacePtr &dspOsci,
                                        const QList<TestDspInterfacePtr> &dspThd)
{
    fireDftActualValues(dspDft);
    fireFftActualValues(dspFft);
    fireRangeActualValues(dspRange);
    fireRmsActualValues(dspRms);
    firePower1ActualValues(dspPower1);
    fireOsciActualValues(dspOsci);
    fireThdActualValues(dspThd);
}
