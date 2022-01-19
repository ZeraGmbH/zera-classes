#include "sourcedevice.h"
#include "io-device/iodevicedemo.h"
#include "json/jsonstructureloader.h"

SourceDevice::SourceDevice(tIoDeviceShPtr interface, SupportedSourceTypes type, QString name, QString version) :
    m_ioInterface(interface),
    m_ioGroupGenerator(JsonStructureLoader::loadJsonStructure(type, name, version)),
    m_type(type),
    m_name(name),
    m_version(version)
{
    m_ioQueue.setIoInterface(interface);

    connect(&m_ioQueue, &IoQueue::sigTransferGroupFinished,
            this, &SourceDevice::onIoGroupFinished);
    connect(this, &SourceDevice::sigSwitchTransationStartedQueued,
            this, &SourceDevice::sigSwitchTransationStarted,
            Qt::QueuedConnection);
    connect(m_ioInterface.get(), &IODeviceBaseSerial::sigDisconnected,
            this, &SourceDevice::sigInterfaceDisconnected,
            Qt::QueuedConnection);
}

SourceDevice::~SourceDevice()
{
}

int SourceDevice::startTransaction(IoTransferDataGroup transferGroup)
{
    doDemoTransactionAdjustments(transferGroup);
    if(transferGroup.isSwitchGroup()) {
        emit sigSwitchTransationStartedQueued();
    }
    if(m_demoSimulErrorActive && !transferGroup.m_ioTransferList.isEmpty()) {
        transferGroup.m_ioTransferList[0]->m_demoErrorResponse = true;
    }
    return m_ioQueue.enqueueTransferGroup(transferGroup);
}

void SourceDevice::simulateExternalDisconnect()
{
    m_ioInterface->simulateExternalDisconnect();
}

void SourceDevice::setDemoResponseDelay(bool followsTimeout, int fixedMs)
{
    m_demoDelayFollowsTimeout = followsTimeout;
    m_demoDelayFixedMs = fixedMs;
}

void SourceDevice::setDemoResonseError(bool active)
{
    m_demoSimulErrorActive = active;
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
    return m_ioInterface->getDeviceInfo();
}

bool SourceDevice::isIoBusy() const
{
    return m_ioQueue.isIoBusy();
}

bool SourceDevice::isDemo() const
{
    return m_ioInterface->isDemo();
}

void SourceDevice::onIoGroupFinished(IoTransferDataGroup transferGroup)
{
    notifyObservers(transferGroup);
}

void SourceDevice::doDemoTransactionAdjustments(const IoTransferDataGroup &transferGroup)
{
    if(isDemo()) {
        IoDeviceDemo* demoInterface = static_cast<IoDeviceDemo*>(m_ioInterface.get());
        demoInterface->setResponseDelay(m_demoDelayFollowsTimeout, m_demoDelayFixedMs);
    }
}
