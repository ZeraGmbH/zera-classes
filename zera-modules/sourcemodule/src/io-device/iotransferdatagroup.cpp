#include "iotransferdatagroup.h"

bool IoTransferDataGroup::passedAll() const
{
    return m_bPassedAll;
}

void IoTransferDataGroup::evalAll()
{
    bool pass = true;
    for(const auto &io : m_ioTransferList) {
        if(io->m_IoEval != IoTransferDataSingle::EVAL_PASS) {
            pass = false;
        }
    }
    m_bPassedAll = pass;
}

bool IoTransferDataGroup::isSwitchGroup() const
{
    return m_commandType == COMMAND_SWITCH_ON || m_commandType == COMMAND_SWITCH_OFF;
}

bool IoTransferDataGroup::isStateQueryGroup() const
{
    return m_commandType == COMMAND_STATE_POLL;
}

bool IoTransferDataGroup::operator ==(const IoTransferDataGroup &other)
{
    return  m_groupId == other.m_groupId &&
            m_commandType == other.m_commandType &&
            m_errorBehavior == other.m_errorBehavior &&
            m_ioTransferList == other.m_ioTransferList;
}

