#ifndef CHANNELADJSTORAGE_H
#define CHANNELADJSTORAGE_H

class ChannelAdjStorage
{
public:
    double getLastGainAdjAmplitude() const;
    void setLastGainAdjAmplitude(double lastGainAdjAmplitude);
    double getLastOffsetAdjAmplitude() const;
    void setLastOffsetAdjAmplitude(double lastOffsetAdjAmplitude);
private:
    double m_lastGainAdjAmplitude = 0.0;
    double m_lastOffsetAdjAmplitude = 0.0;
};

#endif // CHANNELADJSTORAGE_H
