#include "iointerfacedemo.h"

IoInterfaceDemo::IoInterfaceDemo(QObject *parent) : IoInterfaceBase(parent)
{
    m_responseDelayTimer.setSingleShot(true);
    connect(&m_responseDelayTimer, &QTimer::timeout,
            this, &IoInterfaceDemo::onResponseDelayTimer);
}

void IoInterfaceDemo::onResponseDelayTimer()
{
    sendResponse(false);
}

void IoInterfaceDemo::sendResponse(bool interfaceError)
{
    if(interfaceError) {
        m_responseList.clear();
    }
    QByteArray response;
    if(!m_responseList.isEmpty()) {
        response = m_responseList.takeFirst();
    }
    if(m_pDataReceive) {
        *m_pDataReceive = response;
    }
    emit sigIoFinishedToQueue(m_currIoId.getCurrent(), interfaceError);
}

bool IoInterfaceDemo::open(QString strDeviceInfo)
{
    m_strDeviceInfo = strDeviceInfo;
    m_bOpen = true;
    return true;
}

void IoInterfaceDemo::close()
{
    m_bOpen = false;
}

int IoInterfaceDemo::sendAndReceive(QByteArray, QByteArray* pDataReceive)
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

void IoInterfaceDemo::setReadTimeoutNextIo(int timeoutMs)
{
    m_responseDelayMsTimeoutSimul = timeoutMs/2;
    if(m_responseDelayMsTimeoutSimul > 3000) {
        m_responseDelayMsTimeoutSimul /= 3;
    }
}

void IoInterfaceDemo::simulateExternalDisconnect()
{
    emit sigDisconnected();
}

void IoInterfaceDemo::setResponseDelay(bool followsTimeout, int iFixedMs)
{
    m_delayFollowsTimeout = followsTimeout;
    m_responseDelayMs = iFixedMs;
}

void IoInterfaceDemo::appendResponses(QList<QByteArray> responseList)
{
    m_responseList.append(responseList);
}

QList<QByteArray>& IoInterfaceDemo::getResponsesForErrorInjection()
{
    return m_responseList;
}
