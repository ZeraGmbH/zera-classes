#include "sourcedevicefacadetemplate.h"
#include "iodevicetypes.h"
#include "vfmodulecomponent.h"
#include "vfmoduleparameter.h"
#include "messagetexts.h"
#include <jsonparamvalidator.h>
#include "jsonstructureloader.h"

IoIdGenerator SourceDeviceFacadeTemplate::m_idGenerator;

SourceDeviceFacadeTemplate::SourceDeviceFacadeTemplate(IoDeviceBase::Ptr ioDevice, ISourceIo::Ptr sourceIo) :
    m_ioDevice(ioDevice),
    m_sourceIo(sourceIo),
    m_ID(m_idGenerator.nextID())
{
    m_deviceStatusJsonApi.setDeviceInfo(m_ioDevice->getDeviceInfo());
}

void SourceDeviceFacadeTemplate::setVeinInterface(SourceVeinInterface *veinInterface)
{
    m_veinInterface = veinInterface;
    setVeinParamStructure(JsonStructureLoader::loadJsonStructure(m_sourceIo->getProperties()));
    setVeinParamState(m_switcher->getCurrLoadState().getParams());
    setVeinDeviceState(m_deviceStatusJsonApi.getJsonStatus());
    connect(m_veinInterface, &SourceVeinInterface::sigNewLoadParams,
            this, &SourceDeviceFacadeTemplate::switchLoad);
}

int SourceDeviceFacadeTemplate::getId()
{
    return m_ID;
}

QString SourceDeviceFacadeTemplate::getIoDeviceInfo() const
{
    return m_ioDevice->getDeviceInfo();
}

bool SourceDeviceFacadeTemplate::hasDemoIo() const
{
    return m_ioDevice->getType() == IoDeviceTypes::DEMO;
}

QStringList SourceDeviceFacadeTemplate::getLastErrors() const
{
    return m_deviceStatusJsonApi.getErrors();
}

void SourceDeviceFacadeTemplate::setVeinParamStructure(QJsonObject paramStruct)
{
    if(m_veinInterface) {
        m_veinInterface->getVeinDeviceInfoComponent()->setValue(paramStruct);
        m_veinInterface->getVeinDeviceParameterValidator()->setJSonParameterStructure(paramStruct);
    }
}

void SourceDeviceFacadeTemplate::setVeinDeviceState(QJsonObject deviceState)
{
    if(m_veinInterface)
        m_veinInterface->getVeinDeviceStateComponent()->setValue(deviceState);
}

void SourceDeviceFacadeTemplate::setVeinParamState(QJsonObject paramState)
{
    if(m_veinInterface) {
        m_veinInterface->getVeinDeviceParameterComponent()->setValue(paramState);
    }
}

void SourceDeviceFacadeTemplate::resetVeinComponents()
{
    if(m_veinInterface) {
        setVeinParamStructure(QJsonObject());
        setVeinParamState(QJsonObject());
        setVeinDeviceState(QJsonObject());
        disconnect(m_veinInterface, &SourceVeinInterface::sigNewLoadParams,
                   this, &SourceDeviceFacadeTemplate::switchLoad);
        m_veinInterface = nullptr;
    }
}

void SourceDeviceFacadeTemplate::switchLoad(QJsonObject params)
{
    JsonParamApi paramApi;
    paramApi.setParams(params);
    m_switcher->switchState(paramApi);
}

void SourceDeviceFacadeTemplate::onSourceStateChanged(SourceStateController::States state)
{
    if(state == SourceStateController::States::SWITCH_BUSY) {
        m_deviceStatusJsonApi.clearWarningsErrors();
        m_deviceStatusJsonApi.setBusy(true);
    }
    else if(state == SourceStateController::States::IDLE) {
        m_deviceStatusJsonApi.setBusy(false);
    }
    else {
        handleErrorState(state);
    }
    setVeinDeviceState(m_deviceStatusJsonApi.getJsonStatus());
}

void SourceDeviceFacadeTemplate::handleErrorState(SourceStateController::States state)
{
    // All errors need love: translation / helpful status messages
    if(state == SourceStateController::States::ERROR_SWITCH) {
        MessageTexts::Texts msgTxtId = m_switcher->getRequestedLoadState().getOn() ?
                                           MessageTexts::ERR_SWITCH_ON :
                                           MessageTexts::ERR_SWITCH_OFF;
        m_deviceStatusJsonApi.addError(MessageTexts::getText(msgTxtId));
    }
    else if(state == SourceStateController::States::ERROR_POLL) {
        m_deviceStatusJsonApi.addError(MessageTexts::getText(MessageTexts::ERR_STATUS_POLL));
    }

    else {
        qCritical("Unhandled source state!");
    }
    m_deviceStatusJsonApi.setBusy(false);
}

