#include <QJsonDocument>
#include <veinmoduleactvalue.h>
#include <veinmoduleparameter.h>
#include <zera-json-params-structure.h>
#include <jsonparamvalidator.h>
#include "sourcedevice.h"
#include "sourceveininterface.h"
#include "sourceiopacketgenerator.h"

namespace SOURCEMODULE
{

bool SourceDevice::m_removeDemoByDisconnect = false;

SourceDevice::SourceDevice(tSourceInterfaceShPtr interface, SupportedSourceTypes type, QString name, QString version) :
    SourceDeviceBase(interface, type, name, version)
{
    m_persistentParamState = new SourcePersistentJsonState(type, name, version);
    m_paramsCurrent.setParams(m_persistentParamState->loadJsonState());
    m_sourceIoWorker.setIoInterface(interface); // for quick error tests: comment this line
    m_deviceStatus.setDeviceInfo(m_ioInterface->getDeviceInfo());

    connect(interface.get(), &cSourceInterfaceBase::sigDisconnected, this, &SourceDevice::onInterfaceClosed);
    connect(&m_sourceIoWorker, &cSourceIoWorker::sigCmdFinished,
            this, &SourceDevice::onSourceCmdFinished);
}

SourceDevice::~SourceDevice()
{
    delete m_persistentParamState;
}

bool SourceDevice::close(QUuid uuid)
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

void SourceDevice::switchLoad(QJsonObject jsonParamsState)
{
    m_deviceStatus.clearWarningsErrors();
    m_deviceStatus.setBusy(true);
    setVeinDeviceState(m_deviceStatus.getJsonStatus());
    switchState(jsonParamsState);
}

QStringList SourceDevice::getLastErrors()
{
    return m_deviceStatus.getErrors();
}

void SourceDevice::setDemoDelayFollowsTimeout(bool demoDelayFollowsTimeout)
{
    m_bDemoDelayFollowsTimeout = demoDelayFollowsTimeout;
}

void SourceDevice::switchOff()
{
    m_paramsCurrent.setOn(false);
    switchState(m_paramsCurrent.getParams());
}

void SourceDevice::doFinalCloseActivities()
{
    setVeinParamStructure(QJsonObject());
    setVeinParamState(QJsonObject());
    setVeinDeviceState(QJsonObject());
    if(m_veinInterface) {
        disconnect(m_veinInterface->getVeinDeviceParameter(), &cVeinModuleParameter::sigValueChanged, this, &SourceDevice::onNewVeinParamStatus);
    }
    emit sigClosed(this, m_closeUuid);
}

void SourceDevice::onNewVeinParamStatus(QVariant paramState)
{
    switchLoad(paramState.toJsonObject());
}

void SourceDevice::onSourceCmdFinished(cWorkerCommandPacket cmdPack)
{
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
            m_paramsCurrent.setParams(m_paramsRequested.getParams());
            saveState();
        }
        setVeinParamState(m_paramsCurrent.getParams());
        setVeinDeviceState(m_deviceStatus.getJsonStatus());
        if(m_closeRequested) {
            doFinalCloseActivities();
        }
    }
}

void SourceDevice::setVeinInterface(cSourceVeinInterface *veinInterface)
{
    m_veinInterface = veinInterface;
    setVeinParamStructure(m_persistentParamState->getJsonStructure());
    setVeinParamState(m_paramsCurrent.getParams());
    setVeinDeviceState(m_deviceStatus.getJsonStatus());
    connect(m_veinInterface->getVeinDeviceParameter(), &cVeinModuleParameter::sigValueChanged, this, &SourceDevice::onNewVeinParamStatus);
}

void SourceDevice::saveState()
{
    m_persistentParamState->saveJsonState(m_paramsCurrent.getParams());
}

void SourceDevice::onInterfaceClosed()
{
    doFinalCloseActivities();
}

void SourceDevice::switchState(QJsonObject state)
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

void SourceDevice::setVeinParamStructure(QJsonObject paramStruct)
{
    if(m_veinInterface) {
        m_veinInterface->getVeinDeviceInfo()->setValue(paramStruct);
        m_veinInterface->getVeinDeviceParameterValidator()->setJSonParameterStructure(paramStruct);
    }
}

void SourceDevice::setVeinParamState(QJsonObject paramState)
{
    if(m_veinInterface) {
        m_veinInterface->getVeinDeviceParameter()->setValue(paramState);
    }
}

void SourceDevice::setVeinDeviceState(QJsonObject deviceState)
{
    if(m_veinInterface) {
        m_veinInterface->getVeinDeviceState()->setValue(deviceState);
    }
}

} // namespace SOURCEMODULE
