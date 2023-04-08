#include "sourcedevicefacade.h"
#include "jsonstructureloader.h"
#include "messagetexts.h"
#include <vfmoduleactvalue.h>
#include <vfmoduleparameter.h>
#include <jsonparamvalidator.h>

#include <QVariant>

IoIdGenerator SourceDeviceFacade::m_idGenerator;

SourceDeviceFacade::SourceDeviceFacade(IoDeviceBase::Ptr ioDevice, SourceProperties properties) :
    m_ioDevice(ioDevice),
    m_sourceIo(ISourceIo::Ptr(new SourceIo(ioDevice, properties))),
    m_transactionNotifierStatus(SourceTransactionStartNotifier::Ptr::create(m_sourceIo)),
    m_transactionNotifierSwitch(SourceTransactionStartNotifier::Ptr::create(m_sourceIo)),
    m_statePoller(SourceStatePeriodicPoller::Ptr::create(m_transactionNotifierStatus)),
    m_stateController(m_transactionNotifierSwitch, m_transactionNotifierStatus, m_statePoller),
    m_switcher(m_sourceIo, m_transactionNotifierSwitch),
    m_ID(m_idGenerator.nextID())
{
    m_deviceStatusJsonApi.setDeviceInfo(m_ioDevice->getDeviceInfo());
    connect(&m_stateController, &SourceStateController::sigStateChanged,
            this, &SourceDeviceFacade::onSourceStateChanged);
    connect(&m_switcher, &SourceSwitchJson::sigSwitchFinished,
            this, &SourceDeviceFacade::onSourceSwitchFinished);
    connect(ioDevice.get(), &IoDeviceBase::sigDisconnected, this,
            &SourceDeviceFacade::onIoDeviceClosed);
}

QString SourceDeviceFacade::getIoDeviceInfo() const
{
    return m_ioDevice->getDeviceInfo();
}

bool SourceDeviceFacade::hasDemoIo() const
{
    return m_ioDevice->getType() == IoDeviceTypes::DEMO;
}

QStringList SourceDeviceFacade::getLastErrors() const
{
    return m_deviceStatusJsonApi.getErrors();
}

bool SourceDeviceFacade::close(QUuid uuid)
{
    bool closeRequested = tryDemoCloseByUsbDisconnect(uuid);
    if(!m_closeRequested && !closeRequested) {
        closeRequested = true;
        enableCloseRequested(uuid);
        if(m_switcher.getCurrLoadState().getOn()) {
            m_switcher.switchOff();
        }
        else {
            doFinalCloseActivities();
        }
    }
    return closeRequested;
}

void SourceDeviceFacade::switchLoad(QJsonObject params)
{
    JsonParamApi paramApi;
    paramApi.setParams(params);
    m_switcher.switchState(paramApi);
}

int SourceDeviceFacade::getId()
{
    return m_ID;
}

void SourceDeviceFacade::onSourceStateChanged(SourceStateController::States state)
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

void SourceDeviceFacade::onSourceSwitchFinished()
{
    setVeinParamState(m_switcher.getCurrLoadState().getParams());
    if(m_closeRequested) {
        doFinalCloseActivities();
    }
}

void SourceDeviceFacade::onIoDeviceClosed()
{
    doFinalCloseActivities();
}

void SourceDeviceFacade::handleErrorState(SourceStateController::States state)
{
    // All errors need love: translation / helpful status messages
    if(state == SourceStateController::States::ERROR_SWITCH) {
        MessageTexts::Texts msgTxtId = m_switcher.getRequestedLoadState().getOn() ?
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

void SourceDeviceFacade::doFinalCloseActivities()
{
    resetVeinComponents();
    emit sigClosed(getId(), m_closeUuid);
}

void SourceDeviceFacade::setVeinInterface(SourceVeinInterface *veinInterface)
{
    m_veinInterface = veinInterface;
    setVeinParamStructure(JsonStructureLoader::loadJsonStructure(m_sourceIo->getProperties()));
    setVeinParamState(m_switcher.getCurrLoadState().getParams());
    setVeinDeviceState(m_deviceStatusJsonApi.getJsonStatus());
    connect(m_veinInterface, &SourceVeinInterface::sigNewLoadParams,
            this, &SourceDeviceFacade::switchLoad);
}

void SourceDeviceFacade::setStatusPollTime(int ms)
{
    m_statePoller->setPollTime(ms);
}

void SourceDeviceFacade::setVeinParamStructure(QJsonObject paramStruct)
{
    if(m_veinInterface) {
        m_veinInterface->getVeinDeviceInfoComponent()->setValue(paramStruct);
        m_veinInterface->getVeinDeviceParameterValidator()->setJSonParameterStructure(paramStruct);
    }
}

void SourceDeviceFacade::setVeinParamState(QJsonObject paramState)
{
    if(m_veinInterface) {
        m_veinInterface->getVeinDeviceParameterComponent()->setValue(paramState);
    }
}

void SourceDeviceFacade::setVeinDeviceState(QJsonObject deviceState)
{
    if(m_veinInterface) {
        m_veinInterface->getVeinDeviceStateComponent()->setValue(deviceState);
    }
}

void SourceDeviceFacade::resetVeinComponents()
{
    if(m_veinInterface) {
        setVeinParamStructure(QJsonObject());
        setVeinParamState(QJsonObject());
        setVeinDeviceState(QJsonObject());
        disconnect(m_veinInterface, &SourceVeinInterface::sigNewLoadParams,
                   this, &SourceDeviceFacade::switchLoad);
    }
}

void SourceDeviceFacade::enableCloseRequested(QUuid uuid)
{
    m_closeRequested = true;
    m_closeUuid = uuid;
}

bool SourceDeviceFacade::m_demoCloseByUsbDisconnect = false;

bool SourceDeviceFacade::tryDemoCloseByUsbDisconnect(QUuid uuid)
{
    bool closeRequested = false;
    if(hasDemoIo() && !m_closeRequested) {
        m_demoCloseByUsbDisconnect = !m_demoCloseByUsbDisconnect;
        if(m_demoCloseByUsbDisconnect) {
            m_closeRequested = true;
            closeRequested = true;
            enableCloseRequested(uuid);
            m_ioDevice->close();
        }
    }
    return closeRequested;
}
