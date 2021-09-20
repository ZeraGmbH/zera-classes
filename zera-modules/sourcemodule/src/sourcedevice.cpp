#include <QFile>
#include "sourcedevice.h"
#include "iointerface.h"

namespace SOURCEMODULE
{

cSourceDevice::cSourceDevice(cIOInterface* interface, SourceType type, QObject *parent) :
    QObject(parent),
    m_IOInterface(interface),
    m_type(type),
    m_deviceName(randomString(4))
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

QString cSourceDevice::capabilities()
{
    QString capabilityFileName;
    switch(m_type) {
    case SOURCE_DEMO:
    case SOURCE_MT3000:
        capabilityFileName = QStringLiteral("://capabilities/MT3000.json");
        break;
    }
    QFile capabilityFile(capabilityFileName);
    capabilityFile.open(QIODevice::Unbuffered | QIODevice::ReadOnly);
    QString capabilityFileContent = capabilityFile.readAll();
    capabilityFile.close();
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
        // TODO
        Q_ASSERT(false);
        break;
    }
}


}
