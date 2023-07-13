#ifndef MEASMODEBROKER_H
#define MEASMODEBROKER_H

#include "measmodeinfo.h"
#include "measmodephasesetstrategy.h"
#include "meassytemchannels.h"
#include <dspchainidgen.h>
#include <QStringList>
#include <QHash>
#include <functional>

typedef std::function<QStringList(int /*dspSelCode */, MeasSystemChannels, DspChainIdGen&)> DspCreationFunc;

struct DspMModeCreateStruct
{
    measmodes measModeId;
    DspCreationFunc dspCmdFunctionGenFuncPointer;
    std::function<MeasModePhaseSetStrategyPtr()> phaseStrategyGenFuncPointer;
};

typedef QHash<QString /*modeName*/, DspMModeCreateStruct> ModeNameFunctionHash;

class MeasModeBroker
{
public:
    MeasModeBroker(const ModeNameFunctionHash functionHash, DspChainIdGen& dspChainGen);
    struct BrokerReturn
    {
        bool isValid() { return dspSelectCode != 0; } // remember: dspCmdList can be empty on modes using x-modes
        QStringList dspCmdList;
        int dspSelectCode = 0;
        MeasModePhaseSetStrategyPtr phaseStrategy;
    };
    BrokerReturn getMeasMode(QString measModeName, MeasSystemChannels measChannelPairList);
private:
    int getNextSelectionCode();
    const ModeNameFunctionHash m_functionHash;
    int m_nextSelectionCode = 32; // For now avoid collisions with enum measmodes
    QHash<measmodes, int /*selCode*/> m_assignedModes;
    DspChainIdGen& m_dspChainGen;
};

#endif // MEASMODEBROKER_H
