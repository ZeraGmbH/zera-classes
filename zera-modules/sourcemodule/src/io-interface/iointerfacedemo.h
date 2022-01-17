#ifndef IOINTERFACEDEMO_H
#define IOINTERFACEDEMO_H

#include "iointerfacebase.h"
#include <QTimer>
#include <QList>
#include <QByteArray>

class IoInterfaceDemo : public IoInterfaceBase
{
    Q_OBJECT
public:
    virtual bool open(QString strDeviceInfo) override;
    virtual void close() override;
    virtual void setReadTimeoutNextIo(int timeoutMs) override;
    virtual int sendAndReceive(QByteArray bytesSend, QByteArray* pDataReceive) override;
    virtual void simulateExternalDisconnect() override;

    void setResponseDelay(bool followsTimeout, int iFixedMs);
    void appendResponses(QList<QByteArray> responseList);

    virtual IoInterfaceTypes type() override { return SOURCE_INTERFACE_DEMO; }
    virtual bool isOpen() override { return m_bOpen; }

protected:
    explicit IoInterfaceDemo(QObject *parent = nullptr);
    friend class IoInterfaceFactory;
private slots:
    void onResponseDelayTimer();
private:
    void sendResponse(bool error);

    bool m_bOpen = false;
    int m_responseDelayMs = 0;
    int m_responseDelayMsTimeoutSimul = sourceDefaultTimeout/2;
    bool m_delayFollowsTimeout = false;
    QTimer m_responseDelayTimer;
    QByteArray* m_pDataReceive = nullptr;
    QList<QByteArray> m_responseList;
};


#endif // IOINTERFACEDEMO_H
