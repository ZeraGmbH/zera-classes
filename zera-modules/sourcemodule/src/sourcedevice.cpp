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
    if(isDemo()) {
        static_cast<cSourceInterfaceDemo*>(m_ioInterface.get())->simulateExternalDisconnect();
    }
    else {
        // TODO - maybe some sequence?
        qWarning("Close source for non demo is not implemented yet");
    }
}

void cSourceDevice::onNewVeinParamStatus(QVariant paramState)
{
    m_deviceStatus.setBusy(true);
    m_veinInterface->getVeinDeviceState()->setValue(m_deviceStatus.getJsonStatus());

    m_paramsRequested.setParams(paramState.toJsonObject());
    cSourceCommandPacket cmdPack = m_outInGenerator->generateOnOffPacket(m_paramsRequested);
    cWorkerCommandPacket workerPack = SourceWorkerConverter::commandPackToWorkerPack(cmdPack);
    if(isDemo()) {
        cSourceInterfaceDemo* demoInterface = static_cast<cSourceInterfaceDemo*>(getIoInterface().get());
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
        m_paramsCurrent.setParams(m_paramsRequested.getParams());
        saveState();
        m_veinInterface->getVeinDeviceParameter()->setValue(m_paramsCurrent.getParams());
        m_veinInterface->getVeinDeviceState()->setValue(m_deviceStatus.getJsonStatus());
    }
}

tSourceInterfaceShPtr cSourceDevice::getIoInterface()
{
    return m_ioInterface;
}

bool cSourceDevice::isDemo()
{
    return m_ioInterface->isDemo();
}

void cSourceDevice::setVeinInterface(cSourceVeinInterface *veinInterface)
{
    m_veinInterface = veinInterface;
    m_veinInterface->getVeinDeviceInfo()->setValue(m_paramStateLoadSave->getJsonStructure());
    m_veinInterface->getVeinDeviceState()->setValue(m_deviceStatus.getJsonStatus());
    m_veinInterface->getVeinDeviceParameter()->setValue(m_paramsCurrent.getParams());
    m_veinInterface->getVeinDeviceParameterValidator()->setJSonParameterStructure(m_paramStateLoadSave->getJsonStructure());

    connect(m_veinInterface->getVeinDeviceParameter(), &cVeinModuleParameter::sigValueChanged, this, &cSourceDevice::onNewVeinParamStatus);
}

void cSourceDevice::saveState()
{
    m_paramStateLoadSave->saveJsonState(m_paramsCurrent.getParams());
}

void cSourceDevice::onInterfaceClosed()
{
    m_veinInterface->getVeinDeviceInfo()->setValue(QJsonObject());
    m_veinInterface->getVeinDeviceParameter()->setValue(QJsonObject());
    m_veinInterface->getVeinDeviceParameterValidator()->setJSonParameterStructure(QJsonObject());
    m_veinInterface->getVeinDeviceState()->setValue(QJsonObject());

    disconnect(m_veinInterface->getVeinDeviceParameter(), &cVeinModuleParameter::sigValueChanged, this, &cSourceDevice::onNewVeinParamStatus);
    emit sigClosed(this);
}

} // namespace SOURCEMODULE
