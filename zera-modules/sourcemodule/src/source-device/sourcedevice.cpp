#include "sourcedevice.h"
#include "io-interface/iointerfacedemo.h"
#include "json/jsonstructureloader.h"

SourceDevice::SourceDevice(tIoInterfaceShPtr interface, SupportedSourceTypes type, QString name, QString version) :
    m_ioInterface(interface),
    m_outInGenerator(JsonStructureLoader::loadJsonStructure(type, name, version)),
    m_type(type),
    m_name(name),
    m_version(version)
{
    m_sourceIoWorker.setIoInterface(interface);

    connect(&m_sourceIoWorker, &IoWorker::sigCmdFinished,
            this, &SourceDevice::onSourceCmdFinished);
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

int SourceDevice::startTransaction(const IoWorkerCmdPack &workerPack)
{
    doDemoTransactionAdjustments(workerPack);
    if(workerPack.isSwitchPack()) {
        emit sigSwitchTransationStartedQueued();
    }
    return m_sourceIoWorker.enqueueCmd(workerPack);
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

IoPacketGenerator SourceDevice::getIoPacketGenerator()
{
    return m_outInGenerator;
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

void SourceDevice::onSourceCmdFinished(IoWorkerCmdPack cmdPack)
{
    notifyObservers(cmdPack);
}

void SourceDevice::doDemoTransactionAdjustments(const IoWorkerCmdPack &workerPack)
{
    if(isDemo()) {
        IoInterfaceDemo* demoInterface = static_cast<IoInterfaceDemo*>(m_ioInterface.get());
        demoInterface->setResponseDelay(m_demoDelayFollowsTimeout, m_demoDelayFixedMs);
        QList<QByteArray> responseList = DemoResponseHelper::generateResponseList(workerPack);
        if(m_demoSimulErrorActive) {
            if(workerPack.isStateQuery()) {
                responseList.first() = "foo";
            }
            else if(workerPack.isSwitchPack()) {
                responseList.last() = "foo";
            }
        }
        demoInterface->appendResponses(responseList);
    }
}
