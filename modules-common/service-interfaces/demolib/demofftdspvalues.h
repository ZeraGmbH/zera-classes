#ifndef DEMOFFTDSPVALUES_H
#define DEMOFFTDSPVALUES_H

#include <QVector>

class DemoFftDspValues
{
public:
    DemoFftDspValues(int channels, int fftLen = 128);
    void setValue(int channelNo, int harmonicNo, float re, float im);
    QVector<float> getDspValues();
private:
    QVector<float> m_dspValues;
    int m_channels;
    int m_fftLen;
};

#endif // DEMOFFTDSPVALUES_H
