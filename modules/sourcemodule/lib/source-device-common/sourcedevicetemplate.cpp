#include "sourcedevicetemplate.h"
#include "iodevicetypes.h"
#include "vfmodulecomponent.h"
#include "vfmoduleparameter.h"
#include "messagetexts.h"
#include <jsonparamvalidator.h>

IoIdGenerator SourceDeviceTemplate::m_idGenerator;

SourceDeviceTemplate::SourceDeviceTemplate(QString deviceInfo, IoDeviceTypes deviceType, const QJsonObject &sourceCapabilities) :
    m_sourceCapabilities(sourceCapabilities),
    m_deviceType(deviceType),
    m_deviceInfo(deviceInfo),
    m_ID(m_idGenerator.nextID())
{
    m_deviceStatusJsonApi.setDeviceInfo(deviceInfo);
}

void SourceDeviceTemplate::setVeinInterface(SourceVeinInterface *veinInterface)
{
    m_veinInterface = veinInterface;
    setVeinParamStructure(m_sourceCapabilities);
    setVeinParamState(m_switcher->getCurrLoadState().getParams());
    setVeinDeviceState(m_deviceStatusJsonApi.getJsonStatus());
    connect(m_veinInterface, &SourceVeinInterface::sigNewLoadParams,
            this, &SourceDeviceTemplate::switchLoad);
}

int SourceDeviceTemplate::getId()
{
    return m_ID;
}

QString SourceDeviceTemplate::getIoDeviceInfo() const
{
    return m_deviceInfo;
}

bool SourceDeviceTemplate::hasDemoIo() const
{
    return m_deviceType == IoDeviceTypes::DEMO;
}

QStringList SourceDeviceTemplate::getLastErrors() const
{
    return m_deviceStatusJsonApi.getErrors();
}

void SourceDeviceTemplate::setVeinParamStructure(QJsonObject sourceCapabilities)
{
    if(m_veinInterface) {
        m_veinInterface->getVeinDeviceInfoComponent()->setValue(sourceCapabilities);
        m_veinInterface->getVeinDeviceParameterValidator()->setJSonParameterStructure(sourceCapabilities);
    }
}

void SourceDeviceTemplate::setVeinDeviceState(QJsonObject deviceState)
{
    if(m_veinInterface)
        m_veinInterface->getVeinDeviceStateComponent()->setValue(deviceState);
}

void SourceDeviceTemplate::setVeinParamState(QJsonObject paramState)
{
    if(m_veinInterface) {
        m_veinInterface->getVeinDeviceParameterComponent()->setValue(paramState);
    }
}

void SourceDeviceTemplate::resetVeinComponents()
{
    if(m_veinInterface) {
        setVeinParamStructure(QJsonObject());
        setVeinParamState(QJsonObject());
        setVeinDeviceState(QJsonObject());
        disconnect(m_veinInterface, &SourceVeinInterface::sigNewLoadParams,
                   this, &SourceDeviceTemplate::switchLoad);
        m_veinInterface = nullptr;
    }
}

void SourceDeviceTemplate::switchLoad(QJsonObject params)
{
    JsonParamApi paramApi;
    paramApi.setParams(params);
    m_switcher->switchState(paramApi);
}

void SourceDeviceTemplate::onSourceStateChanged(SourceStateController::States state)
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

void SourceDeviceTemplate::handleErrorState(SourceStateController::States state)
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

