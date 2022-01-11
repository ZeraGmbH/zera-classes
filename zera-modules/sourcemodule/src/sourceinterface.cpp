
#include "sourceinterface.h"

tSourceInterfaceShPtr SourceInterfaceFactory::createSourceInterface(SourceInterfaceTypes type, QObject *parent)
{
    SourceInterfaceBase* interface = nullptr;
    switch (type) {
    case SOURCE_INTERFACE_BASE:
        interface = new SourceInterfaceBase(parent);
        break;
    case SOURCE_INTERFACE_DEMO:
        interface = new SourceInterfaceDemo(parent);
        break;
    case SOURCE_INTERFACE_ASYNCSERIAL:
        interface = new SourceInterfaceZeraSerial(parent);
        break;
    case SOURCE_INTERFACE_TYPE_COUNT:
        qCritical("Do not use SOURCE_INTERFACE_TYPE_COUNT");
        break;
    }
    return tSourceInterfaceShPtr(interface);
}


SourceInterfaceBase::SourceInterfaceBase(QObject *parent) : QObject(parent)
{
    connect(this, &SourceInterfaceBase::sigIoFinishedToQueue, this, &SourceInterfaceBase::sigIoFinished, Qt::QueuedConnection);
}

SourceInterfaceBase::~SourceInterfaceBase()
{
}

int SourceInterfaceBase::sendAndReceive(QByteArray, QByteArray*)
{
    m_currIoId.setCurrent(m_IDGenerator.nextID());
    emit sigIoFinishedToQueue(m_currIoId.getCurrent(), true);
    return m_currIoId.getCurrent();
}

QString SourceInterfaceBase::getDeviceInfo()
{
    return m_strDeviceInfo;
}

SourceInterfaceDemo::SourceInterfaceDemo(QObject *parent) : SourceInterfaceBase(parent)
{
    m_responseDelayTimer.setSingleShot(true);
    connect(&m_responseDelayTimer, &QTimer::timeout,
            this, &SourceInterfaceDemo::onResponseDelayTimer);
}

void SourceInterfaceDemo::onResponseDelayTimer()
{
    sendResponse(false);
}

void SourceInterfaceDemo::sendResponse(bool error)
{
    if(!error && m_pDataReceive && !m_responseList.isEmpty()) {
        *m_pDataReceive = m_responseList.takeFirst();
    }
    emit sigIoFinishedToQueue(m_currIoId.getCurrent(), error);
}

bool SourceInterfaceDemo::open(QString strDeviceInfo)
{
    m_strDeviceInfo = strDeviceInfo;
    m_bOpen = true;
    return true;
}

void SourceInterfaceDemo::close()
{
    m_bOpen = false;
}

int SourceInterfaceDemo::sendAndReceive(QByteArray, QByteArray* pDataReceive)
{
    m_currIoId.setCurrent(m_IDGenerator.nextID());
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
    return m_currIoId.getCurrent();
}

void SourceInterfaceDemo::setReadTimeoutNextIo(int timeoutMs)
{
    m_responseDelayMsTimeoutSimul = timeoutMs/2;
    if(m_responseDelayMsTimeoutSimul > 3000) {
        m_responseDelayMsTimeoutSimul /= 3;
    }
}

void SourceInterfaceDemo::simulateExternalDisconnect()
{
    emit sigDisconnected();
}

void SourceInterfaceDemo::setResponseDelay(bool followsTimeout, int iFixedMs)
{
    m_delayFollowsTimeout = followsTimeout;
    m_responseDelayMs = iFixedMs;
}

void SourceInterfaceDemo::setResponses(QList<QByteArray> responseList)
{
    m_responseList = responseList;
}

void SourceInterfaceDemo::appendResponses(QList<QByteArray> responseList)
{
    m_responseList.append(responseList);
}

