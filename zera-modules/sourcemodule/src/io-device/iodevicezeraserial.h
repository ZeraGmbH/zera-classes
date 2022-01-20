#ifndef IODEVICEZERASERIAL_H
#define IODEVICEZERASERIAL_H

#include "iodevicebase.h"
#include "filedisappearwatcher.h"

#include <QObject>
#include <QSerialPortAsyncBlock>

class IoDeviceZeraSerial : public IoDeviceBase
{
    Q_OBJECT
public:
    virtual ~IoDeviceZeraSerial();

    virtual bool open(QString strDeviceInfo) override; // e.g "/dev/ttyUSB0"
    virtual void close() override;
    virtual int sendAndReceive(tIoTransferDataSingleShPtr ioTransferData) override;

    virtual bool isOpen() override;

    void setBlockEndCriteriaNextIo(int iBlockLenReceive = 0, QByteArray endBlock = QByteArray());

protected:
    IoDeviceZeraSerial(IoDeviceTypes type);
    virtual void setReadTimeoutNextIo(int timeoutMs) override;

    friend class IoDeviceFactory;

private slots:
    void onIoFinished();
    void onDeviceFileGone(QString);
private:
    void setReadTimeoutNextIoSerial(int iMsReceiveFirst, int iMsBetweenTwoBytes, int iMsMinTotal = 0);
    void _close();

    QSerialPortAsyncBlock m_serialIO;
    FileDisappearWatcher m_disappearWatcher;
    QByteArray m_dataReceived;

    static constexpr int sourceDefaultMsBetweenTwoBytes = 500;

    struct TTimeoutParam
    {
        int iMsReceiveFirst = ioDefaultTimeout;
        int iMsBetweenTwoBytes = sourceDefaultMsBetweenTwoBytes;
        int iMsMinTotal = 0;
    };
    const TTimeoutParam defaultTimeoutParam;
    TTimeoutParam nextTimeoutParam;
    bool nextTimeoutWasSet = false;

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
