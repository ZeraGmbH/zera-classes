#include <QFile>
#include <QDir>
#include <QJsonDocument>
#include "sourcedevice.h"
#include "iointerface.h"

namespace SOURCEMODULE
{

static enum cSourceDevice::SourceType sDemoTypeCounter(cSourceDevice::SOURCE_DEMO);

cSourceDevice::cSourceDevice(cIOInterface* interface, SourceType type, QObject *parent) :
    QObject(parent),
    m_IOInterface(interface),
    m_type(type)
{
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
    }
}

cSourceDevice::~cSourceDevice()
{
}

cZeraJsonParamsStructure *cSourceDevice::paramsStructure()
{
    return &m_ZeraJsonParamsStructure;
}

cZeraJsonParamsState *cSourceDevice::paramsState()
{
    return &m_ZeraJsonParamsState;
}

void cSourceDevice::close()
{
    switch(m_type) {
    case SOURCE_DEMO:
        // no housekeeping
        m_IOInterface->requestExternalDisconnect();
        break;
    case SOURCE_MT3000:
    case SOURCE_MT400_20:
    case SOURCE_DEMO_FG_4PHASE:
    case SOURCE_MT_SINGLE_PHASE:
        // TODO - maybe some sequence?
        qWarning("Using source type not implemented yet");
        break;
    case SOURCE_TYPE_COUNT:
        qFatal("Do not use SOURCE_TYPE_COUNT");
        break;
    }
}

cIOInterface* cSourceDevice::ioInterface()
{
    return m_IOInterface;
}

QJsonObject cSourceDevice::deviceInfo()
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

QJsonObject cSourceDevice::deviceState()
{
    QJsonObject jsonState;
    if(m_ZeraJsonParamsState.isValid()) {
        jsonState = m_ZeraJsonParamsState.state();
    }
    else {
        QString stateFileName = deviceFileName();
        QString statePath(ZC_DEV_STATE_PATH);
        if(!statePath.endsWith("/")) {
            statePath += "/";
        }
        stateFileName = statePath + stateFileName;
        // in case our client calls deviceState first make sure we have a structure
        if(!m_ZeraJsonParamsStructure.isValid()) {
            deviceInfo();
        }
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
            // sane default
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
        m_ZeraJsonParamsState.setState(jsonState);
    }
    return jsonState;
}

void cSourceDevice::onInterfaceClosed(cIOInterface *ioInterface)
{
    switch(m_type) {
    case SOURCE_DEMO:
        delete ioInterface;
        emit sigClosed(this);
        break;
    case SOURCE_MT3000:
    case SOURCE_MT400_20:
    case SOURCE_DEMO_FG_4PHASE:
    case SOURCE_MT_SINGLE_PHASE:
        // TODO
        qWarning("Using source type not implemented yet");
        break;
    case SOURCE_TYPE_COUNT:
        qFatal("Do not use SOURCE_TYPE_COUNT");
        break;
    }
}

QString cSourceDevice::deviceFileName()
{
    QString fileName;
    // If we ever make it to FG, we need dynamic contents...
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
        qFatal("Do not use SOURCE_DEMO");
        break;
    case SOURCE_TYPE_COUNT:
        qFatal("Do not use SOURCE_TYPE_COUNT");
        break;
    }
    return fileName;
}


} // namespace SOURCEMODULE
