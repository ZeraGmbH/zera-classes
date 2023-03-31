#include "iodevicedemo.h"
#include <timerfactoryqt.h>

IoDeviceDemo::IoDeviceDemo(IoDeviceTypes type) :
    IoDeviceBase(type)
{
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
    if(m_allTransfersError) {
        ioTransferData->getDemoResponder()->activateErrorResponse();
    }
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
        m_responseDelayTimer = TimerFactoryQt::createSingleShot(responseDelayMs);
        connect(m_responseDelayTimer.get(), &TimerTemplateQt::sigExpired,
                this, &IoDeviceDemo::onResponseDelayTimer);
        m_responseDelayTimer->start();
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

void IoDeviceDemo::setAllTransfersError(bool error)
{
    m_allTransfersError = error;
}
