#include "sourcedevice.h"
#include "json/jsonstructureloader.h"

SourceDevice::SourceDevice(IoDeviceBase::Ptr ioDevice, SourceProperties sourceProperties) :
    m_ioDevice(ioDevice),
    m_ioGroupGenerator(JsonStructureLoader::loadJsonStructure(sourceProperties)),
    m_sourceProperties(sourceProperties)
{
    m_ioQueue.setIoDevice(ioDevice);

    connect(&m_ioQueue, &IoQueue::sigTransferGroupFinished,
            this, &SourceDevice::onIoGroupFinished);
    connect(m_ioDevice.get(), &IoDeviceBase::sigDisconnected,
            this, &SourceDevice::sigIoDeviceDisconnected,
            Qt::QueuedConnection);
}

SourceDevice::~SourceDevice()
{
}

int SourceDevice::startTransaction(IoQueueEntry::Ptr transferGroup)
{
    m_ioQueue.enqueueTransferGroup(transferGroup);
    return transferGroup->getGroupId();
}

void SourceDevice::simulateExternalDisconnect()
{
    m_ioDevice->simulateExternalDisconnect();
}

IoGroupGenerator SourceDevice::getIoGroupGenerator() const
{
    return m_ioGroupGenerator;
}

SourceProperties SourceDevice::getProperties() const
{
    return m_sourceProperties;
}

void SourceDevice::onIoGroupFinished(IoQueueEntry::Ptr transferGroup)
{
    emit sigResponseReceived(transferGroup);
}
