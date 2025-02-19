#ifndef RANGEMEASCHANNELSTATEDATA_H
#define RANGEMEASCHANNELSTATEDATA_H

class RangeMeasChannelStateData
{
public:
    RangeMeasChannelStateData();
    void setInvertedPhaseState(bool inverted);
    bool getInvertedPhaseState() const;
private:
    bool m_invertedPhase = false;
};

#endif // RANGEMEASCHANNELSTATEDATA_H
