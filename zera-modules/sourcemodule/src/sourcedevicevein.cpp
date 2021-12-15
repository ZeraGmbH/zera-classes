#include <QJsonDocument>
#include <veinmoduleactvalue.h>
#include <veinmoduleparameter.h>
#include <zera-json-params-structure.h>
#include <jsonparamvalidator.h>
#include "sourcedevicevein.h"
#include "sourceveininterface.h"
#include "sourceiopacketgenerator.h"

namespace SOURCEMODULE
{

bool SourceDeviceVein::m_removeDemoByDisconnect = false;

SourceDeviceVein::SourceDeviceVein(tSourceInterfaceShPtr interface, SupportedSourceTypes type, QString name, QString version) :
    SourceDeviceBase(interface, type, name, version)
{
    m_persistentParamState = new SourcePersistentJsonState(type, name, version);
    m_paramsCurrent.setParams(m_persistentParamState->loadJsonState());
    m_sourceIoWorker.setIoInterface(interface); // for quick error tests: comment this line
    m_deviceStatus.setDeviceInfo(m_ioInterface->getDeviceInfo());

    connect(interface.get(), &cSourceInterfaceBase::sigDisconnected, this, &SourceDeviceVein::onInterfaceClosed);
}

SourceDeviceVein::~SourceDeviceVein()
{
    delete m_persistentParamState;
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
                static_cast<cSourceInterfaceDemo*>(m_ioInterface.get())->simulateExternalDisconnect();
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
    switchState(jsonParamsState);
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

void SourceDeviceVein::onSourceCmdFinished(cWorkerCommandPacket cmdPack)
{
    SourceDeviceBase::onSourceCmdFinished(cmdPack);
    if(m_currentWorkerID == cmdPack.m_workerId) {
        m_deviceStatus.setBusy(false);
        if(!cmdPack.passedAll()) {
            // For now just drop a short note. We need a concept
            // how to continue with translations - maybe an RPC called by GUI?
            // Maybe no
            if(m_paramsRequested.getOn()) {
                m_deviceStatus.addError("Switch on failed");
            }
            else {
                m_deviceStatus.addError("Switch off failed");
            }
        }
        else {
            saveState();
        }
        setVeinParamState(m_paramsCurrent.getParams());
        setVeinDeviceState(m_deviceStatus.getJsonStatus());
        if(m_closeRequested) {
            doFinalCloseActivities();
        }
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
    m_persistentParamState->saveJsonState(m_paramsCurrent.getParams());
}

void SourceDeviceVein::onInterfaceClosed()
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

} // namespace SOURCEMODULE
