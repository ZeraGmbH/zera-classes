#include "ioqueueentry.h"

IoIdGenerator IoQueueEntry::m_idGenerator;

IoQueueEntry::IoQueueEntry(IoQueueErrorBehaviors errorBehavior) :
    m_errorBehavior(errorBehavior),
    m_groupId(m_idGenerator.nextID())
{
}

void IoQueueEntry::appendTransferList(tIoTransferList transferList)
{
    m_ioTransferList.append(transferList);
}

bool IoQueueEntry::passedAll() const
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

int IoQueueEntry::getGroupId() const
{
    return m_groupId;
}

IoQueueErrorBehaviors IoQueueEntry::getErrorBehavior() const
{
    return m_errorBehavior;
}

int IoQueueEntry::getTransferCount()
{
    return m_ioTransferList.count();
}

IoTransferDataSingle::Ptr IoQueueEntry::getTransfer(int idx)
{
    IoTransferDataSingle::Ptr transfer = nullptr;
    if(idx >= 0 && idx < m_ioTransferList.count()) {
        transfer = m_ioTransferList[idx];
    }
    return transfer;
}

