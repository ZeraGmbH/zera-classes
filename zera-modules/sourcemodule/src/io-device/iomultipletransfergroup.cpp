#include "iomultipletransfergroup.h"

bool IoMultipleTransferGroup::passedAll() const
{
    return m_bPassedAll;
}

void IoMultipleTransferGroup::evalAll()
{
    bool pass = true;
    for(auto io : m_ioTransferList) {
        if(io.m_IoEval != IOSingleTransferData::EVAL_PASS) {
            pass = false;
        }
    }
    m_bPassedAll = pass;
}

bool IoMultipleTransferGroup::isSwitchGroup() const
{
    return m_commandType == COMMAND_SWITCH_ON || m_commandType == COMMAND_SWITCH_OFF;
}

bool IoMultipleTransferGroup::isStateQueryGroup() const
{
    return m_commandType == COMMAND_STATE_POLL;
}

bool IoMultipleTransferGroup::operator ==(const IoMultipleTransferGroup &other)
{
    return  m_groupId == other.m_groupId &&
            m_commandType == other.m_commandType &&
            m_errorBehavior == other.m_errorBehavior &&
            m_ioTransferList == other.m_ioTransferList;
}

