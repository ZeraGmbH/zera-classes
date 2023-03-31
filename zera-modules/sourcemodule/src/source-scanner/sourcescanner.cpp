#include "sourcescanner.h"

SourceScanner::Ptr SourceScanner::create(IoDeviceBase::Ptr ioDevice, ISourceScannerStrategy::Ptr ioStrategy, QUuid uuid)
{
    // SourceScanner::Ptr::create does not work because of
    // SourceScanner::SourceScanner is protected for SourceScanner::Ptr
    SourceScanner::Ptr scanner = SourceScanner::Ptr(new SourceScanner(ioDevice, ioStrategy, uuid));
    scanner->m_safePoinerOnThis = scanner;
    return scanner;
}

SourceScanner::SourceScanner(IoDeviceBase::Ptr ioDevice, ISourceScannerStrategy::Ptr ioStrategy, QUuid uuid) :
    m_ioStrategy(ioStrategy),
    m_ioDevice(ioDevice),
    m_uuid(uuid)
{
    m_ioQueue.setIoDevice(m_ioDevice);
    connect(&m_ioQueue, &IoQueue::sigTransferGroupFinished,
            this, &SourceScanner::onTransferGroupFinished,
            Qt::QueuedConnection);
}

void SourceScanner::startScan()
{
    if(m_ioDevice->isOpen()) {
        m_ioQueueList = m_ioStrategy->getIoQueueGroupsForScan();
        for(auto queueEntry : m_ioQueueList) {
            m_pendingQueueEntries.setPending(m_ioQueue.enqueueTransferGroup(queueEntry));
        }
    }
    else {
        finishScan();
    }
}

void SourceScanner::finishScan()
{
    emit sigScanFinished(m_safePoinerOnThis);
    // My job is done. If my connectors died I can do same
    m_safePoinerOnThis = nullptr;
}

void SourceScanner::onTransferGroupFinished(IoQueueGroup::Ptr transferGroup)
{
    m_SourcePropertiesFound = m_ioStrategy->evalResponses(transferGroup);
    bool scanComplete = false;
    if(m_SourcePropertiesFound.wasSet()) {
        scanComplete = true;
    }
    else {
        m_pendingQueueEntries.isPendingAndRemoveIf(transferGroup->getGroupId());
        scanComplete = !m_pendingQueueEntries.hasPending();
    }
    if(scanComplete) {
        finishScan();
    }
}

SourceProperties SourceScanner::getSourcePropertiesFound() const
{
    return m_SourcePropertiesFound;
}

IoDeviceBase::Ptr SourceScanner::getIoDevice()
{
    return m_ioDevice;
}

QUuid SourceScanner::getUuid() const
{
    return m_uuid;
}
