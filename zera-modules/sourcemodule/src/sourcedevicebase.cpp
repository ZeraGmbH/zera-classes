#include "sourcedevicebase.h"
#include "jsonstructureloader.h"
#include "sourcejsonapi.h"

SourceDeviceBase::SourceDeviceBase(tSourceInterfaceShPtr interface, SupportedSourceTypes type, QString deviceName, QString version) :
    QObject(nullptr),
    m_ioInterface(interface)
{
    QJsonObject paramStructure = JsonStructureLoader::loadJsonStructure(type, deviceName, version);
    m_outInGenerator = new SourceIoPacketGenerator(paramStructure);

    connect(&m_sourceIoWorker, &SourceIoWorker::sigCmdFinished,
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
    SourceCommandPacket cmdPack = m_outInGenerator->generateOnOffPacket(m_paramsRequested);
    SourceWorkerCmdPack workerPack = SourceWorkerConverter::commandPackToWorkerPack(cmdPack);
    if(isDemo()) {
        SourceInterfaceDemo* demoInterface = static_cast<SourceInterfaceDemo*>(m_ioInterface.get());
        demoInterface->setResponseDelay(m_bDemoDelayFollowsTimeout, 0);
        QList<QByteArray> responseList = SourceDemoHelper::generateResponseList(workerPack);
        demoInterface->setResponses(responseList);
    }
    m_currWorkerId.setCurrent(m_sourceIoWorker.enqueueAction(workerPack));
}

void SourceDeviceBase::switchOff()
{
    m_paramsCurrent.setOn(false);
    switchState(m_paramsCurrent.getParams());
}

void SourceDeviceBase::handleSourceCmd(SourceWorkerCmdPack cmdPack)
{
    if(cmdPack.passedAll()) {
        m_paramsCurrent.setParams(m_paramsRequested.getParams());
    }
}

void SourceDeviceBase::onSourceCmdFinished(SourceWorkerCmdPack cmdPack)
{
    if(m_currWorkerId.isCurrAndDeactivateIf(cmdPack.m_workerId)) {
        handleSourceCmd(cmdPack);
    }
}
