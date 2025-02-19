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

private:
    bool m_invertedPhase = false;
    double m_preScaling = 1.0;
};

#endif // RANGEMEASCHANNELSTATEDATA_H
