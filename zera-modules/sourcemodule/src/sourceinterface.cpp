
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

int cSourceInterfaceBase::sendAndReceive(QByteArray, QByteArray*)
{
    m_currentIoID = m_IDGenerator.nextID();
    emit sigIoFinishedToQueue(m_currentIoID, true);
    return m_currentIoID;
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
    emit sigIoFinishedToQueue(m_currentIoID, error);
}

bool cSourceInterfaceDemo::open(QString)
{
    m_bOpen = true;
    return true;
}

void cSourceInterfaceDemo::close()
{
    m_bOpen = false;
}

int cSourceInterfaceDemo::sendAndReceive(QByteArray, QByteArray* pDataReceive)
{
    m_currentIoID = m_IDGenerator.nextID();
    m_pDataReceive = pDataReceive;
    int responseDelayMs = m_responseDelayMs;
    if(m_delayFollowsTimeout) {
        responseDelayMs = m_responseDelayMsTimeoutSimul;
    }
    m_responseDelayMsTimeoutSimul = sourceDefaultTimeout/2;
    if(!m_bOpen || responseDelayMs == 0) {
        sendResponse(!m_bOpen);
    }
    else {
        m_responseDelayTimer.start(responseDelayMs);
    }
    return m_currentIoID;
}

void cSourceInterfaceDemo::setReadTimeoutNextIo(int timeoutMs)
{
    m_responseDelayMsTimeoutSimul = timeoutMs/2;
}

void cSourceInterfaceDemo::simulateExternalDisconnect()
{
    emit sigDisconnected();
}

void cSourceInterfaceDemo::setResponseDelay(int iMs)
{
    m_responseDelayMs = iMs;
}

void cSourceInterfaceDemo::setDelayFollowsTimeout(bool followTimeout)
{
    m_delayFollowsTimeout = followTimeout;
}

void cSourceInterfaceDemo::setResponses(QList<QByteArray> responseList)
{
    m_responseList = responseList;
}

void cSourceInterfaceDemo::appendResponses(QList<QByteArray> responseList)
{
    m_responseList.append(responseList);
}

