#include "sourcedeviceswitcherjson.h"
#include "json/jsonstructureloader.h"
#include "json/persistentjsonstate.h"

SourceDeviceSwitcherJson::SourceDeviceSwitcherJson(SourceDevice *sourceDevice) :
    SourceDeviceObserver(sourceDevice),
    m_sourceDevice(sourceDevice)
{
    SupportedSourceTypes type = sourceDevice->getType();
    QString name = sourceDevice->getName();
    QString version = sourceDevice->getVersion();
    QJsonObject paramStructure = JsonStructureLoader::loadJsonStructure(
                type, name, version);
    m_outInGenerator = new IoPacketGenerator(paramStructure);
    m_persistentParamState = new PersistentJsonState(type, name, version);
    m_paramsCurrent.setParams(m_persistentParamState->loadJsonState());
    m_paramsRequested.setParams(m_paramsCurrent.getParams());
    connect(this, &SourceDeviceSwitcherJson::sigBusyChangedQueued,
            this, &SourceDeviceSwitcherJson::sigBusyChanged,
            Qt::QueuedConnection);
}

SourceDeviceSwitcherJson::~SourceDeviceSwitcherJson()
{
    delete m_outInGenerator;
}

void SourceDeviceSwitcherJson::switchState(QJsonObject state)
{
    m_paramsRequested.setParams(state);
    IoCommandPacket cmdPack = m_outInGenerator->generateOnOffPacket(m_paramsRequested);
    IoWorkerCmdPack workerPack = IoWorkerConverter::commandPackToWorkerPack(cmdPack);
    m_sourceDevice->startTransaction(workerPack);
    emit sigBusyChangedQueued(true);
}

void SourceDeviceSwitcherJson::switchOff()
{
    m_paramsRequested.setParams(m_paramsCurrent.getParams());
    m_paramsRequested.setOn(false);
    switchState(m_paramsRequested.getParams());
}

QJsonObject SourceDeviceSwitcherJson::getCurrParamState()
{
    return m_paramsCurrent.getParams();
}

void SourceDeviceSwitcherJson::updateResponse(IoWorkerCmdPack cmdPack)
{
    if(cmdPack.m_commandType == COMMAND_SWITCH) {
        emit sigBusyChanged(false);
        if(cmdPack.passedAll()) {
            m_paramsCurrent.setParams(m_paramsRequested.getParams());
            emit sigCurrParamTouched();
        }
    }
}
