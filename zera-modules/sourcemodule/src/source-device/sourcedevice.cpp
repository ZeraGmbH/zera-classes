#include "sourcedevice.h"
#include "json/jsonstructureloader.h"

SourceDevice::SourceDevice(IoDeviceBase::Ptr ioDevice, SupportedSourceTypes type, QString name, QString version) :
    m_ioDevice(ioDevice),
    m_ioGroupGenerator(JsonStructureLoader::loadJsonStructure(type, name, version)),
    m_type(type),
    m_name(name),
    m_version(version)
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

SupportedSourceTypes SourceDevice::getType() const
{
    return m_type;
}

QString SourceDevice::getName() const
{
    return m_name;
}

QString SourceDevice::getVersion() const
{
    return m_version;
}

QString SourceDevice::getInterfaceInfo() const
{
    return m_ioDevice->getDeviceInfo();
}

bool SourceDevice::isIoBusy() const
{
    return m_ioQueue.isIoBusy();
}

void SourceDevice::onIoGroupFinished(IoTransferDataGroup transferGroup)
{
    notifyObservers(transferGroup);
}
