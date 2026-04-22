#include "sourcescanner.h"

SourceScanner::Ptr SourceScanner::create(IoDeviceBase::Ptr ioDevice, SourceScannerTemplate::Ptr ioStrategy, QUuid uuid)
{
    // SourceScanner::Ptr::create does not work because of
    // SourceScanner::SourceScanner is protected for SourceScanner::Ptr
    SourceScanner::Ptr scanner = SourceScanner::Ptr(new SourceScanner(ioDevice, std::move(ioStrategy), uuid));
    scanner->m_safePoinerOnThis = scanner;
    return scanner;
}

SourceScanner::SourceScanner(IoDeviceBase::Ptr ioDevice, SourceScannerTemplate::Ptr ioStrategy, QUuid uuid) :
    m_ioStrategy(std::move(ioStrategy)),
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
    if(m_ioDevice->isOpen())
        startNextScan();
    else
        finishScan();
}

void SourceScanner::finishScan()
{
    emit sigScanFinished(m_safePoinerOnThis);
    // My job is done. If my connectors died I can do same
    m_safePoinerOnThis = nullptr;
}

void SourceScanner::onTransferGroupFinished(IoQueueGroup::Ptr transferGroup)
{
    Q_UNUSED(transferGroup)
    m_SourcePropertiesFound = m_ioStrategy->findSourceFromResponse();
    if(m_SourcePropertiesFound.wasSet())
        finishScan();
    else
        startNextScan();
}

void SourceScanner::startNextScan()
{
    IoQueueGroup::Ptr nextQueueGroup = m_ioStrategy->getNextQueueGroupForScan();
    if(nextQueueGroup)
        m_ioQueue.enqueueTransferGroup(nextQueueGroup);
    else
        finishScan();
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
