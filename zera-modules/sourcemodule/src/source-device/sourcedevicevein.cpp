#include "sourcedevicevein.h"
#include "message-texts/messagetexts.h"
#include "vein/sourceveininterface.h"
#include "source-protocols/iogroupgenerator.h"
#include <veinmoduleactvalue.h>
#include <veinmoduleparameter.h>
#include <zera-json-params-structure.h>
#include <jsonparamvalidator.h>

bool SourceDeviceVein::m_removeDemoByDisconnect = false;

SourceDeviceVein::SourceDeviceVein(IoDeviceBase::Ptr ioDevice, SupportedSourceTypes type, QString name, QString version) :
    SourceDeviceBase(ioDevice, type, name, version)
{
    init(ioDevice, type, name, version);
}

SourceDeviceVein::SourceDeviceVein(IoDeviceBase::Ptr ioDevice, SourceProperties properties) :
    SourceDeviceBase(ioDevice, properties.getType(), properties.getName(), properties.getVersion())
{
    init(ioDevice, properties.getType(), properties.getName(), properties.getVersion());
}

SourceDeviceVein::~SourceDeviceVein()
{
    delete m_persistentParamState;
}

void SourceDeviceVein::init(IoDeviceBase::Ptr ioDevice, SupportedSourceTypes type, QString name, QString version)
{
    m_persistentParamState = new PersistentJsonState(type, name, version);
    m_paramsCurrent = m_persistentParamState->loadJsonState();
    m_ioQueue.setIoDevice(ioDevice); // for quick error tests: comment this line
    m_deviceStatus.setDeviceInfo(m_ioDevice->getDeviceInfo());

    connect(ioDevice.get(), &IoDeviceBase::sigDisconnected, this, &SourceDeviceVein::onIoDeviceClosed);
}

bool SourceDeviceVein::close(QUuid uuid)
{
    bool closeRequested = false;
    if(!m_closeRequested) {
        if(isDemo()) {
            // Toggle close strategies for test: Call source close / Simulate USB serial removed
            m_removeDemoByDisconnect = !m_removeDemoByDisconnect;
            if(m_removeDemoByDisconnect) {
                closeRequested = true;
                m_closeUuid = uuid;
                m_ioDevice->close();
            }
        }
        if(!closeRequested) {
            closeRequested = true;
            m_closeUuid = uuid;
            m_deviceStatus.clearWarningsErrors();
            if(m_paramsCurrent.getOn()) {
                switchOff();
            }
            else {
                doFinalCloseActivities();
            }
        }
    }
    if(closeRequested) {
        m_closeRequested = true;
    }
    return closeRequested;
}

void SourceDeviceVein::switchVeinLoad(QJsonObject jsonParamsState)
{
    m_deviceStatus.clearWarningsErrors();
    m_deviceStatus.setBusy(true);
    setVeinDeviceState(m_deviceStatus.getJsonStatus());
    JsonParamApi oaramApi;
    oaramApi.setParams(jsonParamsState);
    switchState(oaramApi);
}

QStringList SourceDeviceVein::getLastErrors()
{
    return m_deviceStatus.getErrors();
}

void SourceDeviceVein::doFinalCloseActivities()
{
    setVeinParamStructure(QJsonObject());
    setVeinParamState(QJsonObject());
    setVeinDeviceState(QJsonObject());
    if(m_veinInterface) {
        disconnect(m_veinInterface->getVeinDeviceParameter(), &cVeinModuleParameter::sigValueChanged, this, &SourceDeviceVein::onNewVeinParamStatus);
    }
    emit sigClosed(this, m_closeUuid);
}

void SourceDeviceVein::onNewVeinParamStatus(QVariant paramState)
{
    switchVeinLoad(paramState.toJsonObject());
}

void SourceDeviceVein::handleSourceCmd(IoQueueEntry::Ptr transferGroup)
{
    SourceDeviceBase::handleSourceCmd(transferGroup);
    m_deviceStatus.setBusy(false);
    if(!transferGroup->passedAll()) {
        // For now just drop a short note. We need a concept
        // how to continue with translations - maybe an RPC called by GUI?
        // Maybe no
        if(m_paramsRequested.getOn()) {
            m_deviceStatus.addError(MessageTexts::getText(MessageTexts::ERR_SWITCH_ON));
        }
        else {
            m_deviceStatus.addError(MessageTexts::getText(MessageTexts::ERR_SWITCH_OFF));
        }
    }
    else {
        saveState();
    }
    setVeinDeviceState(m_deviceStatus.getJsonStatus());
    if(m_closeRequested) {
        doFinalCloseActivities();
    }
}

void SourceDeviceVein::setVeinInterface(SourceVeinInterface *veinInterface)
{
    m_veinInterface = veinInterface;
    setVeinParamStructure(m_persistentParamState->getJsonStructure());
    setVeinParamState(m_paramsCurrent.getParams());
    setVeinDeviceState(m_deviceStatus.getJsonStatus());
    connect(m_veinInterface->getVeinDeviceParameter(), &cVeinModuleParameter::sigValueChanged, this, &SourceDeviceVein::onNewVeinParamStatus);
}

void SourceDeviceVein::saveState()
{
    m_persistentParamState->saveJsonState(m_paramsCurrent);
}

void SourceDeviceVein::onIoDeviceClosed()
{
    doFinalCloseActivities();
}

void SourceDeviceVein::setVeinParamStructure(QJsonObject paramStruct)
{
    if(m_veinInterface) {
        m_veinInterface->getVeinDeviceInfo()->setValue(paramStruct);
        m_veinInterface->getVeinDeviceParameterValidator()->setJSonParameterStructure(paramStruct);
    }
}

void SourceDeviceVein::setVeinParamState(QJsonObject paramState)
{
    if(m_veinInterface) {
        m_veinInterface->getVeinDeviceParameter()->setValue(paramState);
    }
}

void SourceDeviceVein::setVeinDeviceState(QJsonObject deviceState)
{
    if(m_veinInterface) {
        m_veinInterface->getVeinDeviceState()->setValue(deviceState);
    }
}

