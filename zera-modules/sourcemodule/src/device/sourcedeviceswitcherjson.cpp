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
    m_paramsCurrent = m_persistentParamState->loadJsonState();
    m_paramsRequested = m_paramsCurrent;
    connect(this, &SourceDeviceSwitcherJson::sigBusyChangedQueued,
            this, &SourceDeviceSwitcherJson::sigBusyChanged,
            Qt::QueuedConnection);
}

SourceDeviceSwitcherJson::~SourceDeviceSwitcherJson()
{
    delete m_outInGenerator;
}

void SourceDeviceSwitcherJson::switchState(JsonParamApi paramState)
{
    m_paramsRequested = paramState;
    IoCommandPacket cmdPack = m_outInGenerator->generateOnOffPacket(m_paramsRequested);
    IoWorkerCmdPack workerPack = IoWorkerConverter::commandPackToWorkerPack(cmdPack);
    m_sourceDevice->startTransaction(workerPack);
    emit sigBusyChangedQueued(true);
}

void SourceDeviceSwitcherJson::switchOff()
{
    m_paramsRequested = m_paramsCurrent;
    m_paramsRequested.setOn(false);
    switchState(m_paramsRequested);
}

JsonParamApi SourceDeviceSwitcherJson::getCurrParamState()
{
    return m_paramsCurrent;
}

void SourceDeviceSwitcherJson::updateResponse(IoWorkerCmdPack cmdPack)
{
    if(cmdPack.m_commandType == COMMAND_SWITCH) {
        emit sigBusyChanged(false);
        if(cmdPack.passedAll()) {
            m_paramsCurrent = m_paramsRequested;
            emit sigCurrParamTouched();
        }
    }
}
