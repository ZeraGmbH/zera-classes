#include "ioqueuegroup.h"

IoIdGenerator IoQueueGroup::m_idGenerator;

IoQueueGroup::IoQueueGroup(IoQueueErrorBehaviors errorBehavior) :
    m_errorBehavior(errorBehavior),
    m_groupId(m_idGenerator.nextID())
{
}

void IoQueueGroup::appendTransferList(tIoTransferList transferList)
{
    m_ioTransferList.append(transferList);
}

bool IoQueueGroup::passedAll() const
{
    bool pass = true;
    for(const auto &io : m_ioTransferList) {
        if(!io->didIoPass()) {
            pass = false;
            break;
        }
    }
    return pass;
}

int IoQueueGroup::getGroupId() const
{
    return m_groupId;
}

IoQueueErrorBehaviors IoQueueGroup::getErrorBehavior() const
{
    return m_errorBehavior;
}

int IoQueueGroup::getTransferCount()
{
    return m_ioTransferList.count();
}

IoTransferDataSingle::Ptr IoQueueGroup::getTransfer(int idx)
{
    IoTransferDataSingle::Ptr transfer = nullptr;
    if(idx >= 0 && idx < m_ioTransferList.count()) {
        transfer = m_ioTransferList[idx];
    }
    return transfer;
}

