#include "sourceinterface.h"

cSourceInterfaceBase *cSourceInterfaceFactory::createSourceInterface(SourceInterfaceTypes type, QObject *parent)
{
    cSourceInterfaceBase* interface = nullptr;
    switch (type) {
    case SOURCE_INTERFACE_BASE:
        interface = new cSourceInterfaceBase(parent);
        break;
    case SOURCE_INTERFACE_DEMO:
        interface = new cSourceInterfaceDemo(parent);
        break;
    case SOURCE_INTERFACE_ASYNCSERIAL:
        interface = new cSourceInterfaceZeraSerial(parent);
        break;
    case SOURCE_INTERFACE_TYPE_COUNT:
        qCritical("Do not use SOURCE_INTERFACE_TYPE_COUNT");
        break;
    }
    return interface;
}


cSourceInterfaceBase::cSourceInterfaceBase(QObject *parent) : QObject(parent)
{
    connect(this, &cSourceInterfaceBase::sigIoFinishedToQueue, this, &cSourceInterfaceBase::sigIoFinished, Qt::QueuedConnection);
}

cSourceInterfaceBase::~cSourceInterfaceBase()
{
}

void cSourceInterfaceBase::close()
{
}

int cSourceInterfaceBase::sendAndReceive(QByteArray, QByteArray*)
{
    return 0;
}

cSourceInterfaceDemo::cSourceInterfaceDemo(QObject *parent) : cSourceInterfaceBase(parent)
{
}

int cSourceInterfaceDemo::sendAndReceive(QByteArray, QByteArray*)
{
    int ioID = m_IDGenerator.nextID();
    emit sigIoFinishedToQueue(ioID);
    return ioID;
}

void cSourceInterfaceDemo::simulateExternalDisconnect()
{
    emit sigDisconnected();
}

