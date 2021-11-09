#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <veinmoduleactvalue.h>
#include <veinmoduleparameter.h>
#include <zera-json-params-structure.h>
#include <jsonparamvalidator.h>
#include "sourcedevice.h"
#include "sourceveininterface.h"
#include "sourceinterface.h"

namespace SOURCEMODULE
{

static enum cSourceDevice::SourceType sDemoTypeCounter(cSourceDevice::SOURCE_DEMO);

cSourceDevice::cSourceDevice(QSharedPointer<cSourceInterfaceBase> interface, SourceType type, QObject *parent) :
    QObject(parent),
    m_spIoInterface(interface),
    m_type(type)
{
    // Currently we keep source type here as an enum. This should not be the final solution
    // because we'll end up in spagetti when adding more source units.
    // In the future type will be replaced by protocol and cSourceScanner will
    // give us that so in case a parameter json comes in we'll pass it to protocol, that returns
    // with a sequence of I/O transacition which we pass to our interface. That is the plan
    // currently...

    connect(interface.get(), &cSourceInterfaceBase::sigDisconnected, this, &cSourceDevice::onInterfaceClosed);

    // demo only stuff
    if(type == SOURCE_DEMO) {
        int nextDemoType = sDemoTypeCounter;
        nextDemoType++;
        if(nextDemoType == SOURCE_TYPE_COUNT) {
            nextDemoType = 0;
        }
        if(nextDemoType == SOURCE_DEMO) {
            nextDemoType++;
        }
        m_demoType = sDemoTypeCounter = cSourceDevice::SourceType(nextDemoType);
        connect(&m_demoTransactionTimer, &QTimer::timeout, this, &cSourceDevice::timeoutDemoTransaction);
    }
}

cSourceDevice::~cSourceDevice()
{
}

void cSourceDevice::close()
{
    switch(m_type) {
    case SOURCE_DEMO:
        // no housekeeping
        static_cast<cSourceInterfaceDemo*>(m_spIoInterface.get())->simulateExternalDisconnect();
        break;
    default:
        // TODO - maybe some sequence?
        qWarning("Using source type not implemented yet");
        break;
    case SOURCE_TYPE_COUNT:
        qCritical("Do not use SOURCE_TYPE_COUNT");
        break;
    }
}

void cSourceDevice::newVeinParamStatus(QVariant paramState)
{
    m_requestedParamState = paramState.toJsonObject();
    m_deviceStatus.setBusy(true);
    m_veinInterface->veinDeviceState()->setValue(m_deviceStatus.jsonStatus());
    if(m_type == SOURCE_DEMO) {
        if(m_requestedParamState.value("on").toBool()) {
            m_demoTransactionTimer.start(3000);
        }
        else {
            m_demoTransactionTimer.start(1000);
        }
    }
}

void cSourceDevice::timeoutDemoTransaction()
{
    // TODO introduce common place for code below for demo and real transactions
    m_deviceStatus.setBusy(false);
    m_currParamState = m_requestedParamState;
    saveState();
    // TODO add some random warnings and erros
    m_veinInterface->veinDeviceParameter()->setValue(m_currParamState);
    m_veinInterface->veinDeviceState()->setValue(m_deviceStatus.jsonStatus());
}

QSharedPointer<cSourceInterfaceBase> cSourceDevice::ioInterface()
{
    return m_spIoInterface;
}

void cSourceDevice::setVeinInterface(cSourceVeinInterface *veinInterface)
{
    m_veinInterface = veinInterface;
    m_veinInterface->veinDeviceInfo()->setValue(deviceParamStructure());
    m_veinInterface->veinDeviceState()->setValue(m_deviceStatus.jsonStatus());
    m_veinInterface->veinDeviceParameter()->setValue(deviceParamState());
    m_veinInterface->veinDeviceParameterValidator()->setJSonParameterStructure(deviceParamStructure());

    connect(m_veinInterface->veinDeviceParameter(), &cVeinModuleParameter::sigValueChanged, this, &cSourceDevice::newVeinParamStatus);
}

const QJsonObject cSourceDevice::deviceParamStructure()
{
    if(m_jsonParamsStructure.isEmpty()) {
        QString deviceInfoFileName = QStringLiteral("://deviceinfo/") + deviceFileName();
        QFile deviceInfoFile(deviceInfoFileName);
        if(deviceInfoFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly)) {
            QByteArray jsondeviceInfoData = deviceInfoFile.readAll();
            deviceInfoFile.close();

            QJsonObject jsonDeviceInfoStructure = QJsonDocument::fromJson(jsondeviceInfoData).object();
            cZeraJsonParamsStructure jsonParamsStructure;
            // structures are tested at build time so we can trust them
            jsonParamsStructure.loadStructure(jsonDeviceInfoStructure);
            m_jsonParamsStructure = jsonParamsStructure.jsonStructure();
        }
    }
    return m_jsonParamsStructure;
}

const QJsonObject cSourceDevice::deviceParamState()
{
    if(m_currParamState.isEmpty()) {
        cZeraJsonParamsState jsonParamsState;
        jsonParamsState.setStructure(deviceParamStructure());
        // try to load persistent state file and validate it
        QFile deviceStateFile(stateFileName());
        if(deviceStateFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly)) {
            QByteArray jsonStateData = deviceStateFile.readAll();
            deviceStateFile.close();
            QJsonObject jsonDeviceStateFromFile = QJsonDocument::fromJson(jsonStateData).object();
            cZeraJsonParamsState::ErrList errList = jsonParamsState.validateJsonState(jsonDeviceStateFromFile);
            if(errList.isEmpty()) {
                // Override on state
                jsonDeviceStateFromFile.insert("on", false);
                m_currParamState = jsonDeviceStateFromFile;
            }
        }
        if(m_currParamState.isEmpty()) {
            // State either not there or corrupt: Heal ourselves and create
            // sane default state file
            m_currParamState = jsonParamsState.createDefaultJsonState();
            saveState();
        }
    }
    return m_currParamState;
}

QString cSourceDevice::stateFileName()
{
    QString fileName = deviceFileName();
    QString statePath(ZC_DEV_STATE_PATH);
    if(!statePath.endsWith("/")) {
        statePath += "/";
    }
    QDir dir; // make sure path exists
    dir.mkpath(statePath);
    fileName = statePath + fileName;
    return fileName;
}

void cSourceDevice::saveState()
{
    QFile deviceStateFile(stateFileName());
    if(deviceStateFile.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(m_currParamState);
        deviceStateFile.write(doc.toJson(QJsonDocument::Indented));
        deviceStateFile.close();
    }
    else {
        qWarning("Default state file %s could not be written", qPrintable(stateFileName()));
    }
}

void cSourceDevice::onInterfaceClosed()
{
    // Once we really have a soft close vein reset has to go there. Now that
    // hard-close by interface close is the only option cleanup vein here
    m_veinInterface->veinDeviceInfo()->setValue(QJsonObject());
    m_veinInterface->veinDeviceParameter()->setValue(QJsonObject());
    m_veinInterface->veinDeviceParameterValidator()->setJSonParameterStructure(QJsonObject());
    m_veinInterface->veinDeviceState()->setValue(QJsonObject());

    // in case interface is gone, there is not much left to do but clean up
    m_spIoInterface = nullptr;
    disconnect(m_veinInterface->veinDeviceParameter(), &cVeinModuleParameter::sigValueChanged, this, &cSourceDevice::newVeinParamStatus);
    emit sigClosed(this);
}

QString cSourceDevice::deviceFileName()
{
    QString fileName;
    // Notes:
    // * If we ever make it to FG, we need dynamic contents...
    // * As written above, these type of decisions should go somwhere else e.g
    //   cSourceScanner
    switch(m_type != SOURCE_DEMO ? m_type : m_demoType) {
    case SOURCE_MT3000:
        fileName = QStringLiteral("MT3000.json");
        break;
    case SOURCE_MT400_20:
        fileName = QStringLiteral("MT400-20.json");
        break;
    case SOURCE_DEMO_FG_4PHASE:
        fileName = QStringLiteral("FG4PhaseTest.json");
        break;
    case SOURCE_MT507:
        fileName = QStringLiteral("MT507.json");
        break;

    case SOURCE_DEMO:
        qCritical("Do not use SOURCE_DEMO");
        break;
    case SOURCE_TYPE_COUNT:
        qCritical("Do not use SOURCE_TYPE_COUNT");
        break;
    }
    return fileName;
}


} // namespace SOURCEMODULE
