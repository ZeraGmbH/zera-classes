#include "sourceioextserial.h"
#include "jsonstructureloader.h"

SourceIoExtSerial::SourceIoExtSerial(IoDeviceBase::Ptr ioDevice, const SourceProperties &sourceProperties) :
    m_ioDevice(ioDevice),
    m_capabilities(JsonStructureLoader::loadJsonStructure(sourceProperties))
{
    m_ioQueue.setIoDevice(ioDevice);

    connect(&m_ioQueue, &IoQueue::sigTransferGroupFinished,
            this, &SourceIoExtSerial::onIoGroupFinished);
}

void SourceIoExtSerial::startTransaction(IoQueueGroup::Ptr transferGroup)
{
    m_ioQueue.enqueueTransferGroup(transferGroup);
}

QJsonObject SourceIoExtSerial::getCapabilities() const
{
    return m_capabilities;
}

void SourceIoExtSerial::onIoGroupFinished(IoQueueGroup::Ptr transferGroup)
{
    emit sigResponseReceived(transferGroup);
}
