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

cSourceDevice::cSourceDevice(tSourceInterfaceShPtr interface, SupportedSourceTypes type, QString version) :
    QObject(nullptr),
    m_ioInterface(interface),
    m_type(type),
    m_version(version)
{
    m_paramStateLoadSave = new cSourceJsonStateIo(type);
    m_outInGenerator = new cSourceIoPacketGenerator(m_paramStateLoadSave->getJsonStructure());
    m_paramsCurrent.setParams(m_paramStateLoadSave->loadJsonState());
    m_sourceIoWorker.setIoInterface(interface); // for quick error tests: comment this line
    m_deviceStatus.setDeviceInfo(m_ioInterface->getDeviceInfo());

    connect(interface.get(), &cSourceInterfaceBase::sigDisconnected, this, &cSourceDevice::onInterfaceClosed);
    connect(&m_sourceIoWorker, &cSourceIoWorker::sigCmdFinished,
            this, &cSourceDevice::onSourceCmdFinished);
}

cSourceDevice::~cSourceDevice()
{
    delete m_outInGenerator;
    delete m_paramStateLoadSave;
}

void cSourceDevice::close()
{
    bool closeRequested = false;
    if(isDemo()) {
        // Toggle close strategies for test: Call source close / Simulate USB serial removed
        m_removeDemoByDisconnect = !m_removeDemoByDisconnect;
        if(m_removeDemoByDisconnect) {
            static_cast<cSourceInterfaceDemo*>(m_ioInterface.get())->simulateExternalDisconnect();
            closeRequested = true;
        }
    }
    if(!closeRequested) {
        // TODO - maybe some sequence?
        qWarning("Close source for non demo is not implemented yet");
    }
}

void cSourceDevice::onNewVeinParamStatus(QVariant paramState)
{
    m_deviceStatus.clearWarningsErrors();
    m_deviceStatus.setBusy(true);
    setVeinDeviceState(m_deviceStatus.getJsonStatus());

    m_paramsRequested.setParams(paramState.toJsonObject());
    cSourceCommandPacket cmdPack = m_outInGenerator->generateOnOffPacket(m_paramsRequested);
    cWorkerCommandPacket workerPack = SourceWorkerConverter::commandPackToWorkerPack(cmdPack);
    if(isDemo()) {
        cSourceInterfaceDemo* demoInterface = static_cast<cSourceInterfaceDemo*>(m_ioInterface.get());
        demoInterface->setDelayFollowsTimeout(true);
        QList<QByteArray> responseList = SourceDemoHelper::generateResponseList(workerPack);
        demoInterface->setResponses(responseList);
    }
    m_currentWorkerID = m_sourceIoWorker.enqueueAction(workerPack);
}

void cSourceDevice::onSourceCmdFinished(cWorkerCommandPacket cmdPack)
{
    if(m_currentWorkerID == cmdPack.m_workerId) {
        m_deviceStatus.setBusy(false);
        if(!cmdPack.passedAll()) {
            // For now just drop a short note. We need a concept
            // how to continue with translations - maybe an RPC called by GUI?
            // Maybe no
            m_deviceStatus.addError("Switch failed");
        }
        m_paramsCurrent.setParams(m_paramsRequested.getParams());
        saveState();
        setVeinParamState(m_paramsCurrent.getParams());
        setVeinDeviceState(m_deviceStatus.getJsonStatus());
    }
}

bool cSourceDevice::isDemo()
{
    return m_ioInterface->isDemo();
}

QString cSourceDevice::getInterfaceDeviceInfo()
{
    return m_ioInterface->getDeviceInfo();
}

void cSourceDevice::setVeinInterface(cSourceVeinInterface *veinInterface)
{
    m_veinInterface = veinInterface;
    setVeinParamStructure(m_paramStateLoadSave->getJsonStructure());
    setVeinParamState(m_paramsCurrent.getParams());
    setVeinDeviceState(m_deviceStatus.getJsonStatus());
    connect(m_veinInterface->getVeinDeviceParameter(), &cVeinModuleParameter::sigValueChanged, this, &cSourceDevice::onNewVeinParamStatus);
}

void cSourceDevice::saveState()
{
    m_paramStateLoadSave->saveJsonState(m_paramsCurrent.getParams());
}

void cSourceDevice::onInterfaceClosed()
{
    setVeinParamStructure(QJsonObject());
    setVeinParamState(QJsonObject());
    setVeinDeviceState(QJsonObject());
    if(m_veinInterface) {
        disconnect(m_veinInterface->getVeinDeviceParameter(), &cVeinModuleParameter::sigValueChanged, this, &cSourceDevice::onNewVeinParamStatus);
    }
    emit sigClosed(this);
}

void cSourceDevice::setVeinParamStructure(QJsonObject paramStruct)
{
    if(m_veinInterface) {
        m_veinInterface->getVeinDeviceInfo()->setValue(paramStruct);
        m_veinInterface->getVeinDeviceParameterValidator()->setJSonParameterStructure(paramStruct);
    }
}

void cSourceDevice::setVeinParamState(QJsonObject paramState)
{
    if(m_veinInterface) {
        m_veinInterface->getVeinDeviceParameter()->setValue(paramState);
    }
}

void cSourceDevice::setVeinDeviceState(QJsonObject deviceState)
{
    if(m_veinInterface) {
        m_veinInterface->getVeinDeviceState()->setValue(deviceState);
    }
}

} // namespace SOURCEMODULE
