#include "measmodebroker.h"

MeasModeBroker::MeasModeBroker(const ModeNameFunctionHash &functionHash) :
    m_functionHash(functionHash)
{
}

MeasModeBroker::BrokerReturn MeasModeBroker::getMeasMode(QString measModeName, MeasSystemChannels measChannelPairList)
{
    BrokerReturn ret;
    if(m_functionHash.contains(measModeName)) {
        DspMModeCreateStruct createStruct = m_functionHash[measModeName];
        if(!m_assignedModes.contains(createStruct.id)) {
            ret.dspSelectCode = getNextSelectionCode();
            ret.dspCmdList = createStruct.func(ret.dspSelectCode, measChannelPairList);
            ret.phaseStrategy = createStruct.phaseStrategyGen();
            m_assignedModes[createStruct.id] = ret.dspSelectCode;
        }
        else
            ret.dspSelectCode = m_assignedModes[createStruct.id];
    }
    return ret;
}

int MeasModeBroker::getNextSelectionCode()
{
    return m_nextSelectionCode++;
}
