#ifndef DEMORANGEDSPVALUES_H
#define DEMORANGEDSPVALUES_H

#include <QVector>

class DemoRangeDspValues
{
public:
    DemoRangeDspValues(int channels);
    void setRmsValue(int channel, float rmsValue);
    void setFrequency(float frequency);
    QVector<float> getDspValues();
private:
    int m_channels;
    QVector<float> m_peakValues;
    QVector<float> m_rmsValues;
    QVector<float> m_freqValue;
    QVector<float> m_dcValues;
};

#endif // DEMORANGEDSPVALUES_H
