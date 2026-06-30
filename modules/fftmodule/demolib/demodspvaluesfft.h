#ifndef DEMODSPVALUESFFT_H
#define DEMODSPVALUESFFT_H

#include <QVector>

class DemoDspValuesFft
{
public:
    DemoDspValuesFft(int channels, int fftLen = 128);
    void setValue(int channelNo, int harmonicNo, float re, float im);
    QVector<float> getDspValues();
    int getFFtLen() const;
private:
    QVector<float> m_dspValues;
    int m_channels;
    int m_fftLen;
};

#endif // DEMODSPVALUESFFT_H
