#include "sourcedevice.h"

SourceDevice::SourceDevice(tSourceInterfaceShPtr interface, SupportedSourceTypes type, QString name, QString version) :
    m_ioInterface(interface),
    m_type(type),
    m_name(name),
    m_version(version)
{
    m_sourceIoWorker.setIoInterface(interface);
    connect(&m_sourceIoWorker, &SourceIoWorker::sigCmdFinished,
            this, &SourceDevice::onSourceCmdFinished);
    connect(m_ioInterface.get(), &SourceInterfaceBase::sigDisconnected,
            this, &SourceDevice::sigInterfaceDisconnected,
            Qt::QueuedConnection);
}

SourceDevice::~SourceDevice()
{
}

int SourceDevice::startTransaction(const SourceWorkerCmdPack &workerPack)
{
    if(isDemo()) {
        SourceInterfaceDemo* demoInterface = static_cast<SourceInterfaceDemo*>(m_ioInterface.get());
        demoInterface->setResponseDelay(m_demoDelayFollowsTimeout, m_demoDelayFixedMs);
        QList<QByteArray> responseList = SourceDemoHelper::generateResponseList(workerPack);
        demoInterface->setResponses(responseList);
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

void SourceDevice::onSourceCmdFinished(SourceWorkerCmdPack cmdPack)
{
    notifyObservers(cmdPack);
}
