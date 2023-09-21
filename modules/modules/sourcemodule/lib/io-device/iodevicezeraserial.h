#ifndef IODEVICEZERASERIAL_H
#define IODEVICEZERASERIAL_H

#include "iodevicebase.h"
#include "filedisappearwatcher.h"
#include <QObject>
#include <QSerialPortAsyncBlock>

static constexpr int serialIoDefaultTimeout = 1500;
static constexpr int serialIoDefaultMsBetweenTwoBytes = 500;

class IoDeviceZeraSerial : public IoDeviceBase
{
    Q_OBJECT
public:
    IoDeviceZeraSerial(IoDeviceTypes type);
    virtual ~IoDeviceZeraSerial();
    virtual bool open(QString strDeviceInfo) override; // e.g "/dev/ttyUSB0"
    virtual void close() override;
    virtual bool isOpen() override;
    virtual int sendAndReceive(IoTransferDataSingle::Ptr ioTransferData) override;
    void setDefaultTimeout(int defaultTimeoutMs);
    void setBlockEndCriteriaNextIo(int iBlockLenReceive = 0, QByteArray endBlock = QByteArray());

protected:
    virtual void setReadTimeoutNextIo(int timeoutMs) override;

private slots:
    void onIoFinished();
    void onDeviceFileGone(QString);
private:
    void setReadTimeoutNextIoSerial(int iMsReceiveFirst, int iMsBetweenTwoBytes, int iMsMinTotal = 0);
    void setReadTimeout();
    void setBlockEndCriteria();
    void _close();
    QSerialPortAsyncBlock m_serialIO;
    FileDisappearWatcher m_disappearWatcher;
    QByteArray m_bytesReceived;
    struct TTimeoutParam
    {
        int iMsReceiveFirst = serialIoDefaultTimeout;
        int iMsBetweenTwoBytes = serialIoDefaultMsBetweenTwoBytes;
        int iMsMinTotal = 0;
    };
    const TTimeoutParam defaultTimeoutParam;
    TTimeoutParam nextTimeoutParam;
    bool nextTimeoutWasSet = false;
    int m_defaultTimeoutMs = serialIoDefaultTimeout;
    struct TBlockEndCriteria
    {
        int iBlockLenReceive = 0;
        QByteArray endBlock = QByteArray("\r");
    };
    const TBlockEndCriteria defaultBlockEndCriteria;
    TBlockEndCriteria nextBlockEndCriteria;
    bool nextBlockEndCriteriaWasSet = false;
};

#endif // IODEVICEZERASERIAL_H
