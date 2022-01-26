#ifndef IOINTERFACEDEMO_H
#define IOINTERFACEDEMO_H

#include "iodevicebase.h"
#include <QTimer>
#include <QList>
#include <QByteArray>

class IoDeviceDemo : public IoDeviceBase
{
    Q_OBJECT
public:
    IoDeviceDemo(IoDeviceTypes type);

    virtual bool open(QString strDeviceInfo) override;
    virtual void close() override;
    virtual bool isOpen() override { return m_bOpen; }

    virtual int sendAndReceive(IoTransferDataSingle::Ptr ioTransferData) override;
    virtual void simulateExternalDisconnect() override;

    void setResponseDelay(bool followsTimeout, int iFixedMs);

private slots:
    void onResponseDelayTimer();
private:
    void sendResponse(bool ioDeviceError);
    virtual void setReadTimeoutNextIo(int timeoutMs) override;

    bool m_bOpen = false;
    int m_responseDelayMs = 0;
    int m_responseDelayMsTimeoutSimul = ioDefaultTimeout/2;
    bool m_delayFollowsTimeout = false;
    QTimer m_responseDelayTimer;
};


#endif // IOINTERFACEDEMO_H
