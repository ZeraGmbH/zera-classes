#include "iotransferdatagroup.h"

IoIdGenerator IoTransferDataGroup::m_idGenerator;

IoTransferDataGroup::IoTransferDataGroup(IoTransferDataGroup::GroupErrorBehaviors errorBehavior) :
    m_errorBehavior(errorBehavior),
    m_groupId(m_idGenerator.nextID())
{
}

IoTransferDataGroup::~IoTransferDataGroup()
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

IoTransferDataSingle::Ptr IoTransferDataGroup::getTransfer(int idx)
{
    IoTransferDataSingle::Ptr transfer = nullptr;
    if(idx >= 0 && idx < m_ioTransferList.count()) {
        transfer = m_ioTransferList[idx];
    }
    return transfer;
}

void IoTransferDataGroup::setDemoErrorOnTransfer(int idx)
{
    if(idx < m_ioTransferList.count()) {
        m_ioTransferList[idx]->m_demoErrorResponse = true;
    }
}

