#include "demofftdspvalues.h"

DemoFftDspValues::DemoFftDspValues(int channels, int fftLen) :
    m_dspValues(channels * 2* fftLen),
    m_channels(channels),
    m_fftLen(fftLen)
{
}

void DemoFftDspValues::setValue(int channelNo, int harmonicNo, float re, float im)
{
    // see cFftModuleMeasProgram::dataReadDSP() for more details
    Q_ASSERT(channelNo>=0 && channelNo < m_channels);
    Q_ASSERT(harmonicNo>=0 && harmonicNo < m_fftLen);

    float invScale = -m_fftLen;
    int middle = m_fftLen;
    float reScaled = re * invScale;
    float imScaled = im * invScale;
    int sourceDataSize = 2 * m_fftLen;
    int channelOffset = channelNo * sourceDataSize;

    if(harmonicNo == 0)
        m_dspValues[channelOffset] = -reScaled; // DC is not multiplied by -1
    else {
        float reScaledLow = reScaled * 3;
        float reScaledUp = reScaled * 2; // // reScaledLow - reScaledUp = reScaled
        m_dspValues[channelOffset + middle + harmonicNo] = reScaledLow;
        m_dspValues[channelOffset + middle * 2 - harmonicNo] = reScaledUp;

        float imScaledLow = imScaled / 3;
        float imScaledUp = imScaled * 2 / 3; // imScaledLow + imScaledUp = reScaled
        m_dspValues[channelOffset + harmonicNo] = imScaledLow;
        m_dspValues[channelOffset + middle - harmonicNo] = imScaledUp;
    }

}

QVector<float> DemoFftDspValues::getDspValues()
{
    return m_dspValues;
}
