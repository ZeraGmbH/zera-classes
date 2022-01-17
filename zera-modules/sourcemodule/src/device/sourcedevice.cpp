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
    connect(this, &SourceDevice::sigSwitchBusyChangedQueued,
            this, &SourceDevice::sigSwitchBusyChanged,
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
    if(isDemo()) {
        IoInterfaceDemo* demoInterface = static_cast<IoInterfaceDemo*>(m_ioInterface.get());
        demoInterface->setResponseDelay(m_demoDelayFollowsTimeout, m_demoDelayFixedMs);
        QList<QByteArray> responseList = DemoResponseHelper::generateResponseList(workerPack);
        demoInterface->appendResponses(responseList);
    }
    if(workerPack.isSwitchPack()) {
        emit sigSwitchBusyChangedQueued(true);
    }
    return m_sourceIoWorker.enqueueAction(workerPack);
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
    if(cmdPack.isSwitchPack()) {
        emit sigSwitchBusyChanged(false);
    }
    notifyObservers(cmdPack);
}
