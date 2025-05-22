#include "sourceioextserial.h"

SourceIoExtSerial::SourceIoExtSerial(IoDeviceBase::Ptr ioDevice, SourceProperties sourceProperties) :
    m_ioDevice(ioDevice),
    m_sourceProperties(sourceProperties)
{
    m_ioQueue.setIoDevice(ioDevice);

    connect(&m_ioQueue, &IoQueue::sigTransferGroupFinished,
            this, &SourceIoExtSerial::onIoGroupFinished);
}

int SourceIoExtSerial::startTransaction(IoQueueGroup::Ptr transferGroup)
{
    m_ioQueue.enqueueTransferGroup(transferGroup);
    return transferGroup->getGroupId();
}

SourceProperties SourceIoExtSerial::getProperties() const
{
    return m_sourceProperties;
}

void SourceIoExtSerial::onIoGroupFinished(IoQueueGroup::Ptr transferGroup)
{
    emit sigResponseReceived(transferGroup);
}
