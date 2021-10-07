#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include <veinmoduleactvalue.h>
#include <veinmoduleparameter.h>
#include <jsonparamvalidator.h>
#include "sourcedevice.h"
#include "sourceveininterface.h"
#include "iointerface.h"

namespace SOURCEMODULE
{

static enum cSourceDevice::SourceType sDemoTypeCounter(cSourceDevice::SOURCE_DEMO);

cSourceDevice::cSourceDevice(cIOInterface* interface, SourceType type, QObject *parent) :
    QObject(parent),
    m_IOInterface(interface),
    m_type(type)
{
    // Currently we keep source type here as an enum. This should not be the final solution
    // because we'll end up in spagetti when adding more source units.
    // In the future type will be replaced by protocol and cSourceConnectTransaction will
    // give us that so in case a parameter json comes in we'll pass it to protocol, that returns
    // with a sequence of I/O transacition which we pass to our interface. That is the plan
    // currently...

    connect(interface, &cIOInterface::sigDisconnected, this, &cSourceDevice::onInterfaceClosed);

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
        m_IOInterface->requestExternalDisconnect();
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
    m_deviceStatus.setBusy(false);
    m_currParamState = m_requestedParamState;
    // TODO add some random warnings and erros
    m_veinInterface->veinDeviceParameter()->setValue(m_currParamState);
    m_veinInterface->veinDeviceState()->setValue(m_deviceStatus.jsonStatus());
}

cIOInterface* cSourceDevice::ioInterface()
{
    return m_IOInterface;
}

void cSourceDevice::setVeinInterface(cSourceVeinInterface *veinInterface)
{
    m_veinInterface = veinInterface;
    m_veinInterface->veinDeviceInfo()->setValue(deviceParamInfo());
    m_veinInterface->veinDeviceParameter()->setValue(deviceParamState());
    m_veinInterface->veinDeviceParameterValidator()->setJSonParameterState(&m_ZeraJsonParamsStructure);
    m_veinInterface->veinDeviceState()->setValue(m_deviceStatus.jsonStatus());

    connect(m_veinInterface->veinDeviceParameter(), &cVeinModuleParameter::sigValueChanged, this, &cSourceDevice::newVeinParamStatus);
}

const QJsonObject cSourceDevice::deviceParamInfo()
{
    QJsonObject devInfo;
    if(!m_ZeraJsonParamsStructure.isValid()) {
        QString deviceInfoFileName = QStringLiteral("://deviceinfo/") + deviceFileName();
        QFile deviceInfoFile(deviceInfoFileName);
        if(deviceInfoFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly)) {
            QByteArray jsondeviceInfoData = deviceInfoFile.readAll();
            deviceInfoFile.close();

            QJsonObject jsonDeviceInfoStructure = QJsonDocument::fromJson(jsondeviceInfoData).object();
            cZeraJsonParamsStructure::ErrList errList = m_ZeraJsonParamsStructure.loadStructure(jsonDeviceInfoStructure);
            if(errList.isEmpty()) {
                devInfo = m_ZeraJsonParamsStructure.jsonStructure();
            }
            else {
                // TODO remove error handling once we check(create?) structure at build time
                qWarning("Errors occured loading json param structure file %s", qPrintable(deviceInfoFileName));
                while(!errList.isEmpty()) {
                    cZeraJsonParamsStructure::errEntry err = errList.takeFirst();
                    qWarning("%s: %s", qPrintable(err.strID()), qPrintable(err.m_strInfo));
                }
            }
        }
    }
    else {
        devInfo = m_ZeraJsonParamsStructure.jsonStructure();
    }
    return devInfo;
}

const QJsonObject cSourceDevice::deviceParamState()
{
    QJsonObject jsonState;
    if(!m_currParamState.isEmpty()) {
        return m_currParamState;
    }
    else {
        QString stateFileName = deviceFileName();
        QString statePath(ZC_DEV_STATE_PATH);
        if(!statePath.endsWith("/")) {
            statePath += "/";
        }
        stateFileName = statePath + stateFileName;
        // in case our client calls deviceParamState first make sure we have a structure
        if(!m_ZeraJsonParamsStructure.isValid()) {
            deviceParamInfo();
        }
        // try to load state file and validate it
        QFile deviceStateFile(stateFileName);
        if(deviceStateFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly)) {
            QByteArray jsonStateData = deviceStateFile.readAll();
            deviceStateFile.close();
            QJsonObject jsonDeviceStateFromFile = QJsonDocument::fromJson(jsonStateData).object();
            cZeraJsonParamsStructure::ErrList errList = m_ZeraJsonParamsStructure.validateJsonState(jsonDeviceStateFromFile);
            if(errList.isEmpty()) {
                jsonState = jsonDeviceStateFromFile;
            }
        }
        if(jsonState.isEmpty()) {
            // State either not there or corrupt: Heal ourselves and create
            // sane default state file
            QDir dir;
            dir.mkpath(statePath);
            jsonState = m_ZeraJsonParamsStructure.createDefaultJsonState();
            if(deviceStateFile.open(QIODevice::WriteOnly)) {
                QJsonDocument doc(jsonState);
                deviceStateFile.write(doc.toJson(QJsonDocument::Indented));
                deviceStateFile.close();
            }
            else {
                qWarning("Default state file %s could not be written", qPrintable(stateFileName));
            }
        }
        m_currParamState = jsonState;
    }
    return jsonState;
}

void cSourceDevice::onInterfaceClosed(cIOInterface *ioInterface)
{
    // Once we really have a soft close vein reset has to go there. Now that
    // hard-close by interface close is the only option cleanup vein here
    m_veinInterface->veinDeviceInfo()->setValue(QJsonObject());
    m_veinInterface->veinDeviceParameter()->setValue(QJsonObject());
    m_veinInterface->veinDeviceParameterValidator()->setJSonParameterState(nullptr);
    m_veinInterface->veinDeviceState()->setValue(QJsonObject());

    // in case interface is gone, there is not much left to do but clean up
    delete ioInterface;
    disconnect(m_veinInterface->veinDeviceParameter(), &cVeinModuleParameter::sigValueChanged, this, &cSourceDevice::newVeinParamStatus);
    emit sigClosed(this);
}

QString cSourceDevice::deviceFileName()
{
    QString fileName;
    // Notes:
    // * If we ever make it to FG, we need dynamic contents...
    // * As written above, these type of decisions should go somwhere else e.g
    //   cSourceConnectTransaction
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
    case SOURCE_MT_SINGLE_PHASE:
        fileName = QStringLiteral("MT-Single-Phase.json");
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
