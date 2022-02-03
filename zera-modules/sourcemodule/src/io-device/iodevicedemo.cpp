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
        m_ioTransferData->setDataReceived(m_ioTransferData->getDemoResponder()->getDemoResponse());
    }
    emit sigIoFinishedQueued(m_currIoId.getPending(), ioDeviceError);
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
    emit sigDisconnected();
}

int IoDeviceDemo::sendAndReceive(IoTransferDataSingle::Ptr ioTransferData)
{
    prepareSendAndReceive(ioTransferData);
    int responseDelayMs = m_responseDelayMs;
    if(m_delayFollowsTimeout) {
        responseDelayMs = m_responseDelayMsTimeoutSimul;
    }
    // set default for next
    m_responseDelayMsTimeoutSimul = demoIoDefaultTimeout;
    if(!m_bOpen || responseDelayMs == 0) {
        sendResponse(!m_bOpen);
    }
    else {
        m_responseDelayTimer.start(responseDelayMs);
    }
    return m_currIoId.getPending();
}

void IoDeviceDemo::setReadTimeoutNextIo(int timeoutMs)
{
    m_responseDelayMsTimeoutSimul = timeoutMs/2;
    if(m_responseDelayMsTimeoutSimul > 3000) {
        m_responseDelayMsTimeoutSimul /= 3;
    }
}

void IoDeviceDemo::setResponseDelay(bool followsTimeout, int iFixedMs)
{
    m_delayFollowsTimeout = followsTimeout;
    m_responseDelayMs = iFixedMs;
}
