#include "iotransferdatagroup.h"


IoIdGenerator IoTransferDataGroup::m_idGenerator;

IoTransferDataGroup::IoTransferDataGroup() :
    m_groupId(m_idGenerator.nextID())
{
}

IoTransferDataGroup::IoTransferDataGroup(IoTransferDataGroup::SourceGroupTypes groupType,
                                         IoTransferDataGroup::GroupErrorBehaviors errorBehavior) :
    m_groupType(groupType),
    m_errorBehavior(errorBehavior),
    m_groupId(m_idGenerator.nextID())
{
}

void IoTransferDataGroup::appendTransferList(tIoTransferList transferList)
{
    m_ioTransferList.append(transferList);
}

bool IoTransferDataGroup::passedAll() const
{
    return m_bPassedAll;
}

void IoTransferDataGroup::evalAll()
{
    bool pass = true;
    for(const auto &io : m_ioTransferList) {
        if(io->getEvaluation() != IoTransferDataSingle::EVAL_PASS) {
            pass = false;
        }
    }
    m_bPassedAll = pass;
}

bool IoTransferDataGroup::isSwitchGroup() const
{
    return m_groupType == GROUP_TYPE_SWITCH_ON || m_groupType == GROUP_TYPE_SWITCH_OFF;
}

bool IoTransferDataGroup::isStateQueryGroup() const
{
    return m_groupType == GROUP_TYPE_STATE_POLL;
}

IoTransferDataGroup::SourceGroupTypes IoTransferDataGroup::getGroupType() const
{
    return m_groupType;
}

int IoTransferDataGroup::getGroupId() const
{
    return m_groupId;
}

IoTransferDataGroup::GroupErrorBehaviors IoTransferDataGroup::getErrorBehavior() const
{
    return m_errorBehavior;
}

int IoTransferDataGroup::getTransferCount()
{
    return m_ioTransferList.count();
}

tIoTransferDataSingleShPtr IoTransferDataGroup::getTransfer(int idx)
{
    tIoTransferDataSingleShPtr transfer = nullptr;
    if(idx >= 0 && idx < m_ioTransferList.count()) {
        transfer = m_ioTransferList[idx];
    }
    return transfer;
}

bool IoTransferDataGroup::operator ==(const IoTransferDataGroup &other)
{
    return  m_groupId == other.m_groupId &&
            m_groupType == other.m_groupType &&
            m_errorBehavior == other.m_errorBehavior &&
            m_ioTransferList == other.m_ioTransferList;
}

bool IoTransferDataGroup::operator !=(const IoTransferDataGroup &other)
{
    return !(*this == other);
}

void IoTransferDataGroup::setDemoErrorOnTransfer(int idx)
{
    if(idx < m_ioTransferList.count()) {
        m_ioTransferList[idx]->m_demoErrorResponse = true;
    }
}

