#include "sourcedeviceswitcherjson.h"
#include "json/persistentjsonstate.h"

SourceDeviceSwitcherJson::SourceDeviceSwitcherJson(SourceDevice *sourceDevice) :
    SourceDeviceObserver(sourceDevice),
    m_sourceDevice(sourceDevice)
{
    SupportedSourceTypes type = sourceDevice->getType();
    QString name = sourceDevice->getName();
    QString version = sourceDevice->getVersion();

    m_persistentParamState = new PersistentJsonState(type, name, version);
    m_paramsCurrent = m_persistentParamState->loadJsonState();
    m_paramsRequested = m_paramsCurrent;
}

SourceDeviceSwitcherJson::~SourceDeviceSwitcherJson()
{
}

void SourceDeviceSwitcherJson::switchState(JsonParamApi paramState)
{
    m_paramsRequested = paramState;
    IoCommandPacket cmdPack = m_sourceDevice->getIoPacketGenerator()->generateOnOffPacket(m_paramsRequested);
    IoWorkerCmdPack workerPack = IoWorkerConverter::commandPackToWorkerPack(cmdPack);
    m_sourceDevice->startTransaction(workerPack);
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
    if(cmdPack.isSwitchPack()) {
        if(cmdPack.passedAll()) {
            m_paramsCurrent = m_paramsRequested;
            m_persistentParamState->saveJsonState(m_paramsCurrent);
            emit sigCurrParamTouched();
        }
    }
}
