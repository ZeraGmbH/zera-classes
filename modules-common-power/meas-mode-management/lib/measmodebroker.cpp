#include "measmodebroker.h"

MeasModeBroker::MeasModeBroker(const ModeNameFunctionHash functionHash, DspChainIdGen &dspChainGen) :
    m_functionHash(functionHash),
    m_dspChainGen(dspChainGen)
{
}

MeasModeBroker::BrokerReturn MeasModeBroker::getMeasMode(QString measModeName, MeasSystemChannels measChannelPairList)
{
    BrokerReturn ret;
    if(m_functionHash.contains(measModeName)) {
        DspMModeCreateStruct createStruct = m_functionHash[measModeName];
        ret.phaseStrategy = createStruct.phaseStrategyGenFuncPointer();
        if(!m_assignedModes.contains(createStruct.measModeId)) {
            ret.dspSelectCode = getNextSelectionCode();
            ret.dspCmdList = createStruct.dspCmdFunctionGenFuncPointer(ret.dspSelectCode, measChannelPairList, m_dspChainGen);
            m_assignedModes[createStruct.measModeId] = ret.dspSelectCode;
        }
        else
            ret.dspSelectCode = m_assignedModes[createStruct.measModeId];
    }
    return ret;
}

int MeasModeBroker::getNextSelectionCode()
{
    return m_nextSelectionCode++;
}
