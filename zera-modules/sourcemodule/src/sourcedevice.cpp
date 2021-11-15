#include <QJsonDocument>
#include <veinmoduleactvalue.h>
#include <veinmoduleparameter.h>
#include <zera-json-params-structure.h>
#include <jsonparamvalidator.h>
#include "sourcedevice.h"
#include "sourceveininterface.h"
#include "sourceinterface.h"
#include "sourceiotransactiongenerator.h"

namespace SOURCEMODULE
{

cSourceDevice::cSourceDevice(QSharedPointer<cSourceInterfaceBase> interface, SupportedSourceTypes type, QString version) :
    QObject(nullptr),
    m_spIoInterface(interface),
    m_type(type),
    m_version(version)
{
    m_paramStateIo = new cSourceJsonStateIo(m_type);
    m_ioTransactionGenerator.setParamsStructure(m_paramStateIo->getJsonStructure());
    m_currParamState = m_paramStateIo->loadJsonState();

    connect(interface.get(), &cSourceInterfaceBase::sigDisconnected, this, &cSourceDevice::onInterfaceClosed);

    if(isDemo()) {
        m_demoOnOffDelayTimer.setSingleShot(true);
        connect(&m_demoOnOffDelayTimer, &QTimer::timeout, this, &cSourceDevice::onTimeoutDemoTransaction);
    }
}

cSourceDevice::~cSourceDevice()
{
    delete m_paramStateIo;
}

void cSourceDevice::close()
{
    if(isDemo()) {
        static_cast<cSourceInterfaceDemo*>(m_spIoInterface.get())->simulateExternalDisconnect();
    }
    else {
        // TODO - maybe some sequence?
        qWarning("Close source for non demo is not implemented yet");
    }
}

void cSourceDevice::onNewVeinParamStatus(QVariant paramState)
{
    m_requestedParamState = paramState.toJsonObject();
    m_deviceStatus.setBusy(true);
    m_veinInterface->getVeinDeviceState()->setValue(m_deviceStatus.getJsonStatus());
    // transactionList is not necessary for demo but let's create it here for
    // debug purpose
    tSourceIoTransactionList transactionList = m_ioTransactionGenerator.generateIoTransactionList(m_requestedParamState);
    if(isDemo()) {
        if(m_requestedParamState.value("on").toBool()) {
            m_demoOnOffDelayTimer.start(3000);
        }
        else {
            m_demoOnOffDelayTimer.start(1000);
        }
    }
    else {
        startActions(transactionList);
    }
}

void cSourceDevice::onTimeoutDemoTransaction()
{
    m_deviceStatus.setBusy(false);
    m_currParamState = m_requestedParamState;
    saveState();
    // TODO add some random warnings and erros
    m_veinInterface->getVeinDeviceParameter()->setValue(m_currParamState);
    m_veinInterface->getVeinDeviceState()->setValue(m_deviceStatus.getJsonStatus());
}

QSharedPointer<cSourceInterfaceBase> cSourceDevice::getIoInterface()
{
    return m_spIoInterface;
}

bool cSourceDevice::isDemo()
{
    return m_spIoInterface->type() == SOURCE_INTERFACE_DEMO;
}

void cSourceDevice::setVeinInterface(cSourceVeinInterface *veinInterface)
{
    m_veinInterface = veinInterface;
    m_veinInterface->getVeinDeviceInfo()->setValue(m_paramStateIo->getJsonStructure());
    m_veinInterface->getVeinDeviceState()->setValue(m_deviceStatus.getJsonStatus());
    m_veinInterface->getVeinDeviceParameter()->setValue(m_currParamState);
    m_veinInterface->getVeinDeviceParameterValidator()->setJSonParameterStructure(m_paramStateIo->getJsonStructure());

    connect(m_veinInterface->getVeinDeviceParameter(), &cVeinModuleParameter::sigValueChanged, this, &cSourceDevice::onNewVeinParamStatus);
}

void cSourceDevice::startActions(tSourceIoTransactionList transactionList)
{

}

void cSourceDevice::saveState()
{
    m_paramStateIo->saveJsonState(m_currParamState);
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
