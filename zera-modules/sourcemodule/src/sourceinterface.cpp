#include "sourceinterface.h"

int cSourceInterfaceTransactionIdGenerator::nextTransactionID()
{
    if(++m_currentTransActionID == 0) {
        ++m_currentTransActionID;
    }
    return m_currentTransActionID;
}

cSourceInterfaceBase *cSourceInterfaceFactory::createSourceInterface(SourceInterfaceType type, QObject *parent)
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
    connect(this, &cSourceInterfaceBase::ioFinishedToQueue, this, &cSourceInterfaceBase::ioFinished, Qt::QueuedConnection);
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
    int transactionID = m_transactionIDGenerator.nextTransactionID();
    emit ioFinishedToQueue(transactionID);
    return transactionID;
}

void cSourceInterfaceDemo::simulateExternalDisconnect()
{
    emit sigDisconnected(this);
}

