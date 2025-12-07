#ifndef CHANNELCOMMONSTORAGE_H
#define CHANNELCOMMONSTORAGE_H

class ChannelCommonStorage
{
public:
    bool getInvertedPhaseState() const;
    void setInvertedPhaseState(bool inverted);

    double getLastGainAdjAmplitude() const;
    void setLastGainAdjAmplitude(double lastGainAdjAmplitude);

    double getLastOffsetAdjAmplitude() const;
    void setLastOffsetAdjAmplitude(double lastOffsetAdjAmplitude);
private:
    bool m_invertPhase = false;
    double m_lastGainAdjAmplitude = 0.0;
    double m_lastOffsetAdjAmplitude = 0.0;
};

#endif // CHANNELCOMMONSTORAGE_H
