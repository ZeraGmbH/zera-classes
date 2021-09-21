#include <QFile>
#include "sourcedevice.h"
#include "iointerface.h"

namespace SOURCEMODULE
{

cSourceDevice::cSourceDevice(cIOInterface* interface, SourceType type, QObject *parent) :
    QObject(parent),
    m_IOInterface(interface),
    m_type(type)
{
    connect(interface, &cIOInterface::sigDisconnected, this, &cSourceDevice::onInterfaceClosed);
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
        // TODO - maybe some sequence?
        Q_ASSERT(false);
        break;
    }
}

cIOInterface* cSourceDevice::ioInterface()
{
    return m_IOInterface;
}

QString cSourceDevice::name()
{
    return m_deviceName;
}

QString cSourceDevice::deviceInfo()
{
    QString capabilityFileName;
    // If we ever make it to FG, we need dynamic contents...
    switch(m_type) {
    case SOURCE_DEMO:
    case SOURCE_MT3000:
        capabilityFileName = QStringLiteral("://deviceinfo/MT3000.json");
        break;
    case SOURCE_MT400_20:
        capabilityFileName = QStringLiteral("://deviceinfo/MT400-20.json");
        break;
    }
    QFile capabilityFile(capabilityFileName);
    capabilityFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly);
    QString capabilityFileContent = capabilityFile.readAll();
    capabilityFile.close();

    capabilityFileContent.replace("%name%", name());
    capabilityFileContent.replace("\n", "").replace(" ", "");
    return capabilityFileContent;
}


void cSourceDevice::setName(QString name)
{
    m_deviceName = name;
}


void cSourceDevice::onInterfaceClosed(cIOInterface *ioInterface)
{
    switch(m_type) {
    case SOURCE_DEMO:
        delete ioInterface;
        emit sigClosed(this);
        break;
    case SOURCE_MT3000:
        // TODO
        Q_ASSERT(false);
        break;
    }
}


}
