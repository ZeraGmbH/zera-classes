#include "iodevicedemo.h"

IoDeviceDemo::IoDeviceDemo(IoDeviceTypes type) :
    IoDeviceBase(type)
{
    m_responseDelayTimer.setSingleShot(true);
    connect(&m_responseDelayTimer, &QTimer::timeout,
            this, &IoDeviceDemo::onResponseDelayTimer);
}

void IoDeviceDemo::onResponseDelayTimer()
{
    sendResponse(false);
}

void IoDeviceDemo::sendResponse(bool ioDeviceError)
{
    if(!ioDeviceError) {
        m_ioTransferData->m_dataReceived = m_ioTransferData->getDemoResponse();
    }
    emit _sigIoFinished(m_currIoId.getCurrent(), ioDeviceError);
}

bool IoDeviceDemo::open(QString strDeviceInfo)
{
    m_strDeviceInfo = strDeviceInfo;
    m_bOpen = true;
    return true;
}

void IoDeviceDemo::close()
{
    m_bOpen = false;
}

int IoDeviceDemo::sendAndReceive(tIoTransferDataSingleShPtr ioTransferData)
{
    prepareSendAndReceive(ioTransferData);
    int responseDelayMs = m_responseDelayMs;
    if(m_delayFollowsTimeout) {
        responseDelayMs = m_responseDelayMsTimeoutSimul;
    }
    // set default for next
    m_responseDelayMsTimeoutSimul = sourceDefaultTimeout/2;
    if(!m_bOpen || responseDelayMs == 0) {
        sendResponse(!m_bOpen);
    }
    else {
        m_responseDelayTimer.start(responseDelayMs);
    }
    return m_currIoId.getCurrent();
}

void IoDeviceDemo::setReadTimeoutNextIo(int timeoutMs)
{
    m_responseDelayMsTimeoutSimul = timeoutMs/2;
    if(m_responseDelayMsTimeoutSimul > 3000) {
        m_responseDelayMsTimeoutSimul /= 3;
    }
}

void IoDeviceDemo::simulateExternalDisconnect()
{
    emit sigDisconnected();
}

void IoDeviceDemo::setResponseDelay(bool followsTimeout, int iFixedMs)
{
    m_delayFollowsTimeout = followsTimeout;
    m_responseDelayMs = iFixedMs;
}
