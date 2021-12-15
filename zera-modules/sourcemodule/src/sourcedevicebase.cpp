#include "sourcedevicebase.h"
#include "jsonstructureloader.h"
#include "sourcejsonapi.h"

namespace SOURCEMODULE
{

SourceDeviceBase::SourceDeviceBase(tSourceInterfaceShPtr interface, SupportedSourceTypes type, QString deviceName, QString version) :
    QObject(nullptr),
    m_ioInterface(interface)
{
    QJsonObject paramStructure = JsonStructureLoader::loadJsonStructure(type, deviceName, version);
    m_outInGenerator = new cSourceIoPacketGenerator(paramStructure);

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

void SourceDeviceBase::onSourceCmdFinished(cWorkerCommandPacket cmdPack)
{
    if(m_currentWorkerID == cmdPack.m_workerId) {
        if(cmdPack.passedAll()) {
            m_paramsCurrent.setParams(m_paramsRequested.getParams());
        }
    }
}

void SourceDeviceBase::switchState(QJsonObject state)
{
    m_paramsRequested.setParams(state);
    cSourceCommandPacket cmdPack = m_outInGenerator->generateOnOffPacket(m_paramsRequested);
    cWorkerCommandPacket workerPack = SourceWorkerConverter::commandPackToWorkerPack(cmdPack);
    if(isDemo()) {
        cSourceInterfaceDemo* demoInterface = static_cast<cSourceInterfaceDemo*>(m_ioInterface.get());
        demoInterface->setDelayFollowsTimeout(m_bDemoDelayFollowsTimeout);
        QList<QByteArray> responseList = SourceDemoHelper::generateResponseList(workerPack);
        demoInterface->setResponses(responseList);
    }
    m_currentWorkerID = m_sourceIoWorker.enqueueAction(workerPack);
}

void SourceDeviceBase::switchOff()
{
    m_paramsCurrent.setOn(false);
    switchState(m_paramsCurrent.getParams());
}


}
