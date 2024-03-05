#ifndef DEMOVALUESDSPFFT_H
#define DEMOVALUESDSPFFT_H

#include <QVector>

class DemoValuesDspFft
{
public:
    DemoValuesDspFft(int channels, int fftLen = 128);
    void setValue(int channelNo, int harmonicNo, float re, float im);
    QVector<float> getDspValues();
private:
    QVector<float> m_dspValues;
    int m_channels;
    int m_fftLen;
};

#endif // DEMOVALUESDSPFFT_H
