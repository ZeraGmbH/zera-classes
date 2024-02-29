#include "demorangedspvalues.h"
#include <cmath>

DemoRangeDspValues::DemoRangeDspValues(int channels) :
    m_channels(channels)
{
}

void DemoRangeDspValues::setValue(QVector<float> randomChannelRMS, double frequency)
{
    m_demoRangeValues.clear();
    // peak
    for (int channel=0; channel<m_channels; ++channel) {
        double randPeak = randomChannelRMS[channel] * M_SQRT2;
        m_demoRangeValues.append(randPeak);
    }
    // RMS
    for (int channel=0; channel<m_channels; ++channel) {
        double randRms = randomChannelRMS[channel];
        m_demoRangeValues.append(randRms);
    }
    // frequency
    m_demoRangeValues.append(frequency);
    // peak DC (no DC for now)
    for (int channel=0; channel<m_channels; ++channel) {
        double randPeak = randomChannelRMS[channel] * M_SQRT2;
        m_demoRangeValues.append(randPeak);
    }
}

QVector<float> DemoRangeDspValues::getDspValues()
{
    return m_demoRangeValues;
}
