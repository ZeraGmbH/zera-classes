#include "sourcedevice.h"
#include "io-interface/iointerfacedemo.h"
#include "json/jsonstructureloader.h"

SourceDevice::SourceDevice(tIoInterfaceShPtr interface, SupportedSourceTypes type, QString name, QString version) :
    m_ioInterface(interface),
    m_ioGroupGenerator(JsonStructureLoader::loadJsonStructure(type, name, version)),
    m_type(type),
    m_name(name),
    m_version(version)
{
    m_ioWorker.setIoInterface(interface);

    connect(&m_ioWorker, &IoWorker::sigTransferGroupFinished,
            this, &SourceDevice::onIoGroupFinished);
    connect(this, &SourceDevice::sigSwitchTransationStartedQueued,
            this, &SourceDevice::sigSwitchTransationStarted,
            Qt::QueuedConnection);
    connect(m_ioInterface.get(), &IoInterfaceBase::sigDisconnected,
            this, &SourceDevice::sigInterfaceDisconnected,
            Qt::QueuedConnection);
}

SourceDevice::~SourceDevice()
{
}

int SourceDevice::startTransaction(const IoMultipleTransferGroup &transferGroup)
{
    doDemoTransactionAdjustments(transferGroup);
    if(transferGroup.isSwitchGroup()) {
        emit sigSwitchTransationStartedQueued();
    }
    return m_ioWorker.enqueueTransferGroup(transferGroup);
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

bool SourceDevice::isDemo() const
{
    return m_ioInterface->isDemo();
}

void SourceDevice::onIoGroupFinished(IoMultipleTransferGroup transferGroup)
{
    notifyObservers(transferGroup);
}

void SourceDevice::doDemoTransactionAdjustments(const IoMultipleTransferGroup &transferGroup)
{
    if(isDemo()) {
        IoInterfaceDemo* demoInterface = static_cast<IoInterfaceDemo*>(m_ioInterface.get());
        demoInterface->setResponseDelay(m_demoDelayFollowsTimeout, m_demoDelayFixedMs);
        QList<QByteArray> responseList = DemoResponseHelper::generateResponseList(transferGroup);
        if(m_demoSimulErrorActive) {
            responseList.first() = "foo";
        }
        demoInterface->appendResponses(responseList);
    }
}
