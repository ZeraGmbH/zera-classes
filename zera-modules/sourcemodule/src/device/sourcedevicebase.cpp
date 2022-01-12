#include "sourcedevicebase.h"
#include "json/jsonstructureloader.h"
#include "io-interface/iointerfacedemo.h"

SourceDeviceBase::SourceDeviceBase(tIoInterfaceShPtr interface, SupportedSourceTypes type, QString deviceName, QString version) :
    QObject(nullptr),
    m_ioInterface(interface)
{
    QJsonObject paramStructure = JsonStructureLoader::loadJsonStructure(type, deviceName, version);
    m_outInGenerator = new IoPacketGenerator(paramStructure);

    connect(&m_sourceIoWorker, &IoWorker::sigCmdFinished,
            this, &SourceDeviceBase::onSourceCmdFinished);
}

SourceDeviceBase::~SourceDeviceBase()
{
    delete m_outInGenerator;
}

void SourceDeviceBase::setDemoDelayFollowsTimeout(bool demoDelayFollowsTimeout)
{
    m_bDemoDelayFollowsTimeout = demoDelayFollowsTimeout;
}

bool SourceDeviceBase::isDemo()
{
    return m_ioInterface->isDemo();
}

QString SourceDeviceBase::getInterfaceDeviceInfo()
{
    return m_ioInterface->getDeviceInfo();
}

void SourceDeviceBase::switchState(QJsonObject state)
{
    m_paramsRequested.setParams(state);
    IoCommandPacket cmdPack = m_outInGenerator->generateOnOffPacket(m_paramsRequested);
    IoWorkerCmdPack workerPack = IoWorkerConverter::commandPackToWorkerPack(cmdPack);
    if(isDemo()) {
        IoInterfaceDemo* demoInterface = static_cast<IoInterfaceDemo*>(m_ioInterface.get());
        demoInterface->setResponseDelay(m_bDemoDelayFollowsTimeout, 0);
        QList<QByteArray> responseList = DemoResponseHelper::generateResponseList(workerPack);
        demoInterface->setResponses(responseList);
    }
    m_currWorkerId.setCurrent(m_sourceIoWorker.enqueueAction(workerPack));
}

void SourceDeviceBase::switchOff()
{
    m_paramsCurrent.setOn(false);
    switchState(m_paramsCurrent.getParams());
}

void SourceDeviceBase::handleSourceCmd(IoWorkerCmdPack cmdPack)
{
    if(cmdPack.passedAll()) {
        m_paramsCurrent.setParams(m_paramsRequested.getParams());
    }
}

void SourceDeviceBase::onSourceCmdFinished(IoWorkerCmdPack cmdPack)
{
    if(m_currWorkerId.isCurrAndDeactivateIf(cmdPack.m_workerId)) {
        handleSourceCmd(cmdPack);
    }
}
