#ifndef IOINTERFACEDEMO_H
#define IOINTERFACEDEMO_H

#include "iodevicebaseserial.h"
#include <QTimer>
#include <QList>
#include <QByteArray>

class IoDeviceDemo : public IODeviceBaseSerial
{
    Q_OBJECT
public:
    virtual bool open(QString strDeviceInfo) override;
    virtual void close() override;

    virtual void setReadTimeoutNextIo(int timeoutMs) override;

    virtual int sendAndReceive(tIoTransferDataSingleShPtr ioTransferData) override;
    virtual void simulateExternalDisconnect() override;

    virtual bool isOpen() override { return m_bOpen; }

    void setResponseDelay(bool followsTimeout, int iFixedMs);

protected:
    explicit IoDeviceDemo(IoDeviceTypes type);

    friend class IoDeviceFactory;

private slots:
    void onResponseDelayTimer();
private:
    void sendResponse(bool ioDeviceError);

    bool m_bOpen = false;
    int m_responseDelayMs = 0;
    int m_responseDelayMsTimeoutSimul = sourceDefaultTimeout/2;
    bool m_delayFollowsTimeout = false;
    QTimer m_responseDelayTimer;
};


#endif // IOINTERFACEDEMO_H
