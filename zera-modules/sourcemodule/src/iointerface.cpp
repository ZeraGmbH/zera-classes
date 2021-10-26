#include "iointerface.h"

cIOInterface::cIOInterface(cIOInterface::IOType ioType, QString deviceFilename, QObject *parent) :
    QObject(parent),
    m_ioType(ioType),
    m_deviceFilename(deviceFilename)
{
}

cIOInterface::~cIOInterface()
{
}

cIOInterface::IOType cIOInterface::type()
{
    return m_ioType;
}

QString cIOInterface::deviceFileName()
{
    return m_deviceFilename;
}

void cIOInterface::requestExternalDisconnect()
{
    switch(m_ioType) {
    case IO_DEMO:
        emit sigDisconnected(this);
        break;
    case IO_ASYNC_SERIAL:
        Q_ASSERT(false);
        break;
    }

}
