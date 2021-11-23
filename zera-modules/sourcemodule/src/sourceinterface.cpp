
#include "sourceinterface.h"

tSourceInterfaceShPtr cSourceInterfaceFactory::createSourceInterface(SourceInterfaceTypes type, QObject *parent)
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
    return tSourceInterfaceShPtr(interface);
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
    int ioID = m_IDGenerator.nextID();
    emit sigIoFinishedToQueue(ioID, true);
    return ioID;
}

cSourceInterfaceDemo::cSourceInterfaceDemo(QObject *parent) : cSourceInterfaceBase(parent)
{
    m_responseDelayTimer.setSingleShot(true);
    connect(&m_responseDelayTimer, &QTimer::timeout,
            this, &cSourceInterfaceDemo::onResponseDelayTimer);
}

void cSourceInterfaceDemo::onResponseDelayTimer()
{
    sendResponse(false);
}

void cSourceInterfaceDemo::sendResponse(bool error)
{
    if(!error && m_pDataReceive && !m_responseList.isEmpty()) {
        *m_pDataReceive = m_responseList.takeFirst();
    }
    emit sigIoFinishedToQueue(m_currentId, error);
}

bool cSourceInterfaceDemo::open(QString)
{
    m_bOpen = true;
    return true;
}

int cSourceInterfaceDemo::sendAndReceive(QByteArray, QByteArray* pDataReceive)
{
    m_currentId = m_IDGenerator.nextID();
    m_pDataReceive = pDataReceive;
    if(!m_bOpen || m_responseDelayMs == 0) {
        sendResponse(!m_bOpen);
    }
    else {
        m_responseDelayTimer.start(m_responseDelayMs);
    }
    return m_currentId;
}

void cSourceInterfaceDemo::simulateExternalDisconnect()
{
    emit sigDisconnected();
}

void cSourceInterfaceDemo::setResponseDelay(int iMs)
{
    m_responseDelayMs = iMs;
}

void cSourceInterfaceDemo::setResponses(QList<QByteArray> responseList)
{
    m_responseList = responseList;
}

void cSourceInterfaceDemo::appendResponses(QList<QByteArray> responseList)
{
    m_responseList.append(responseList);
}

