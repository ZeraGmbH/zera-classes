#include <QFile>
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

QString cSourceDevice::deviceInfo()
{
    QString capabilityFileName;
    // If we ever make it to FG, we need dynamic contents...
    switch(m_type != SOURCE_DEMO ? m_type : m_demoType) {
    case SOURCE_MT3000:
        capabilityFileName = QStringLiteral("://deviceinfo/MT3000.json");
        break;
    case SOURCE_MT400_20:
        capabilityFileName = QStringLiteral("://deviceinfo/MT400-20.json");
        break;
    case SOURCE_DEMO_FG_4PHASE:
        capabilityFileName = QStringLiteral("://deviceinfo/FG4PhaseTest.json");
        break;

    case SOURCE_DEMO:
        qFatal("Do not use SOURCE_DEMO");
        break;
    case SOURCE_TYPE_COUNT:
        qFatal("Do not use SOURCE_TYPE_COUNT");
        break;
    }
    QFile capabilityFile(capabilityFileName);
    capabilityFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly);
    QString capabilityFileContent = capabilityFile.readAll();
    capabilityFile.close();

    capabilityFileContent.replace("\n", "").replace(" ", "");
    return capabilityFileContent;
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
        // TODO
        qWarning("Using source type not implemented yet");
        break;
    case SOURCE_TYPE_COUNT:
        qFatal("Do not use SOURCE_TYPE_COUNT");
        break;
    }
}


}
