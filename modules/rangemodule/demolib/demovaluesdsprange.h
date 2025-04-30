#ifndef DEMOVALUESDSPRANGE_H
#define DEMOVALUESDSPRANGE_H

#include <QVector>

class DemoValuesDspRange
{
public:
    DemoValuesDspRange(int channels);
    void setRmsPeakDCValue(int channel, float rmsValue);
    void setFrequency(float frequency);
    QVector<float> getDspValues();
private:
    int m_channels;
    QVector<float> m_peakValues;
    QVector<float> m_rmsValues;
    QVector<float> m_freqValue;
    QVector<float> m_dcValues;
};

#endif // DEMOVALUESDSPRANGE_H
