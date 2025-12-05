#ifndef CHANNELADJSTORAGE_H
#define CHANNELADJSTORAGE_H

class ChannelAdjStorage
{
public:
    double getLastAdjAmplitude() const;
    void setLastAdjAmplitude(double lastAdjAmplitude);

    double getLastGainCorrectionInternalAndClamp() const;
    void setLastGainCorrectionInternalAndClamp(double lastGainCorrectionInternalAndClamp);
private:
    double m_lastAdjAmplitude = 0.0;
    double m_lastGainCorrectionInternalAndClamp = 1.0; // reflects DSP GAINCORRECTION1
};

#endif // CHANNELADJSTORAGE_H
