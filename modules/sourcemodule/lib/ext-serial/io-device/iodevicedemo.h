#ifndef IOINTERFACEDEMO_H
#define IOINTERFACEDEMO_H

#include "iodevicebase.h"
#include <timersingleshotqt.h>
#include <QList>
#include <QByteArray>

static constexpr int demoIoDefaultTimeout = 1000;

class IoDeviceDemo : public IoDeviceBase
{
    Q_OBJECT
public:
    IoDeviceDemo(IoDeviceTypes type);

    virtual bool open(QString strDeviceInfo) override;
    virtual void close() override;
    virtual bool isOpen() override { return m_bOpen; }

    virtual int sendAndReceive(IoTransferDataSingle::Ptr ioTransferData) override;

    void setResponseDelay(bool followsTimeout, int iFixedMs);
    void setAllTransfersError(bool error);

private slots:
    void onResponseDelayTimer();
private:
    void sendResponse(bool ioDeviceError);
    virtual void setReadTimeoutNextIo(int timeoutMs) override;

    bool m_bOpen = false;
    int m_responseDelayMs = 0;
    int m_responseDelayMsTimeoutSimul = demoIoDefaultTimeout;
    bool m_delayFollowsTimeout = false;
    TimerTemplateQtPtr m_responseDelayTimer;
    bool m_allTransfersError = false;
};


#endif // IOINTERFACEDEMO_H
