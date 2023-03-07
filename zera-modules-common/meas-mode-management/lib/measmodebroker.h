#ifndef MEASMODEBROKER_H
#define MEASMODEBROKER_H

#include "measmodeinfo.h"
#include "measmodephasesetstrategy.h"
#include "meassytemchannels.h"
#include <QStringList>
#include <QHash>
#include <functional>

typedef std::function<QStringList(int /*dspSelCode */, MeasSystemChannels)> DspCreationFunc;

struct DspMModeCreateStruct
{
    measmodes id;
    DspCreationFunc func;
    std::function<MeasModePhaseSetStrategyPtr()> phaseStrategyGen;
};

typedef QHash<QString /*modeName*/, DspMModeCreateStruct> ModeNameFunctionHash;

class MeasModeBroker
{
public:
    MeasModeBroker(const ModeNameFunctionHash &functionHash);
    struct BrokerReturn
    {
        bool isValid() { return dspSelectCode != 0; }
        QStringList dspCmdList;
        int dspSelectCode = 0;
        MeasModePhaseSetStrategyPtr phaseStrategy;
    };
    BrokerReturn getMeasMode(QString measModeName, MeasSystemChannels measChannelPairList);
private:
    int getNextSelectionCode();
    const ModeNameFunctionHash &m_functionHash;
    int m_nextSelectionCode = 32; // For now avoid collisions with enum measmodes
    QHash<measmodes, int /*selCode*/> m_assignedModes;
};

#endif // MEASMODEBROKER_H
