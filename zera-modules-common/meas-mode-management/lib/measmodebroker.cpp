#include "measmodebroker.h"

MeasModeBroker::MeasModeBroker(const ModeNameFunctionHash &functionHash) :
    m_functionHash(functionHash)
{
}

MeasModeBroker::BrokerReturn MeasModeBroker::getMeasMode(QString measModeName, MeasSystemChannels measChannelPairList)
{
    BrokerReturn ret;
    if(m_functionHash.contains(measModeName)) {
        DspCreateFuncWithId funcIdPair = m_functionHash[measModeName];
        if(!m_assignedModes.contains(funcIdPair.id)) {
            ret.dspSelectCode = getNextSelectionCode();
            ret.dspCmdList = funcIdPair.func(ret.dspSelectCode, measChannelPairList);
            m_assignedModes[funcIdPair.id] = ret.dspSelectCode;
        }
        else
            ret.dspSelectCode = m_assignedModes[funcIdPair.id];
    }
    return ret;
}

int MeasModeBroker::getNextSelectionCode()
{
    return m_nextSelectionCode++;
}
