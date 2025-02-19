#ifndef RANGEMEASCHANNELSTATEDATA_H
#define RANGEMEASCHANNELSTATEDATA_H

class RangeMeasChannelStateData
{
public:
    RangeMeasChannelStateData();
    void setInvertedPhaseState(bool inverted);
    bool getInvertedPhaseState() const;

    double getPreScaling() const;
    void setPreScaling(double preScaling);

    double getRmsValue() const;
    void setRmsValue(double rmsValue);

    double getSignalFrequency() const;
    void setSignalFrequency(double signalFrequency);

private:
    bool m_invertedPhase = false;
    double m_preScaling = 1.0;
    double m_rmsValue = 0.0;
    double m_signalFrequency = 13.0;
};

#endif // RANGEMEASCHANNELSTATEDATA_H
