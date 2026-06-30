#ifndef DEMODSPVALUESRANGE_H
#define DEMODSPVALUESRANGE_H

#include <QVector>

class DemoDspValuesRange
{
public:
    DemoDspValuesRange(int channels);
    void setRmsPeakDCValue(int channel, float rmsValue);
    void setRmsPeakDCValue(int channel, float rmsValue, float peakValue);
    void setFrequency(float frequency);
    QVector<float> getDspValues();
private:
    int m_channels;
    QVector<float> m_peakValues;
    QVector<float> m_rmsValues;
    QVector<float> m_freqValue;
    QVector<float> m_dcValues;
};

#endif // DEMODSPVALUESRANGE_H
