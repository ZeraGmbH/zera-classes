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
    IoTransferDataGroup transferGroup = m_sourceDevice->getIoGroupGenerator().generateOnOffGroup(m_paramsRequested);
    m_sourceDevice->startTransaction(transferGroup);
}

void SourceDeviceSwitcherJson::switchOff()
{
    m_paramsRequested = m_paramsCurrent;
    m_paramsRequested.setOn(false);
    switchState(m_paramsRequested);
}

JsonParamApi SourceDeviceSwitcherJson::getCurrLoadState()
{
    return m_paramsCurrent;
}

void SourceDeviceSwitcherJson::updateResponse(IoTransferDataGroup transferGroup)
{
    if(transferGroup.isSwitchGroup()) {
        if(transferGroup.passedAll()) {
            m_paramsCurrent = m_paramsRequested;
            m_persistentParamState->saveJsonState(m_paramsCurrent);
            emit sigSwitchFinished();
        }
    }
}
