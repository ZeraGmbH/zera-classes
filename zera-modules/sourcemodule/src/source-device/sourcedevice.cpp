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
    connect(this, &SourceDevice::sigSwitchTransationStartedQueued,
            this, &SourceDevice::sigSwitchTransationStarted,
            Qt::QueuedConnection);
    connect(m_ioDevice.get(), &IoDeviceBase::sigDisconnected,
            this, &SourceDevice::sigInterfaceDisconnected,
            Qt::QueuedConnection);
}

SourceDevice::~SourceDevice()
{
}

int SourceDevice::startTransaction(IoTransferDataGroup transferGroup)
{
    if(transferGroup.isSwitchGroup()) {
        emit sigSwitchTransationStartedQueued();
    }
    return m_ioQueue.enqueueTransferGroup(transferGroup);
}

void SourceDevice::simulateExternalDisconnect()
{
    m_ioDevice->simulateExternalDisconnect();
}

IoGroupGenerator SourceDevice::getIoGroupGenerator()
{
    return m_ioGroupGenerator;
}

QString SourceDevice::getInterfaceInfo() const
{
    return m_ioDevice->getDeviceInfo();
}

SourceProperties SourceDevice::getProperties() const
{
    return m_sourceProperties;
}

bool SourceDevice::isIoBusy() const
{
    return m_ioQueue.isIoBusy();
}

void SourceDevice::onIoGroupFinished(IoTransferDataGroup transferGroup)
{
    notifyObservers(transferGroup);
}
