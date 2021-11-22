
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
    return 0;
}

cSourceInterfaceDemo::cSourceInterfaceDemo(QObject *parent) : cSourceInterfaceBase(parent)
{
    m_responseDelayTimer.setSingleShot(true);
    connect(&m_responseDelayTimer, &QTimer::timeout,
            this, &cSourceInterfaceDemo::onResponseDelayTimer);
}

void cSourceInterfaceDemo::onResponseDelayTimer()
{
    sendResponse();
}

void cSourceInterfaceDemo::sendResponse()
{
    if(m_pDataReceive && !m_responseList.isEmpty()) {
        *m_pDataReceive = m_responseList.takeFirst();
    }
    emit sigIoFinishedToQueue(m_currentId);
}

bool cSourceInterfaceDemo::open(QString)
{
    m_bOpen = true;
    return true;
}

int cSourceInterfaceDemo::sendAndReceive(QByteArray, QByteArray* pDataReceive)
{
    m_currentId = 0;
    m_pDataReceive = pDataReceive;
    if(m_bOpen) {
        m_currentId = m_IDGenerator.nextID();
    }
    if(!m_bOpen || m_responseDelayMs == 0) {
        sendResponse();
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

