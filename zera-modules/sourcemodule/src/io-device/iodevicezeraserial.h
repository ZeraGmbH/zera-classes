#ifndef IODEVICEZERASERIAL_H
#define IODEVICEZERASERIAL_H

#include "iodevicebaseserial.h"
#include "filedisappearwatcher.h"

#include <QObject>
#include <QSerialPortAsyncBlock>

class IoDeviceZeraSerialPrivate;
class IoDeviceZeraSerial : public IODeviceBaseSerial
{
    Q_OBJECT
public:
    virtual ~IoDeviceZeraSerial();

    virtual IoDeviceTypes type() override { return SERIAL_DEVICE_ASYNCSERIAL; }
    virtual bool open(QString strDeviceInfo) override; // e.g "/dev/ttyUSB0"
    virtual void close() override;
    virtual void setReadTimeoutNextIo(int timeoutMs) override;
    virtual int sendAndReceive(QByteArray bytesSend, QByteArray* pDataReceive) override;

    // wrappers - see https://github.com/ZeraGmbH/qtiohelper / QT += serialportasyncblock
    void setReadTimeoutNextIo(int iMsReceiveFirst, int iMsBetweenTwoBytes, int iMsMinTotal = 0);
    void setBlockEndCriteriaNextIo(int iBlockLenReceive = 0, QByteArray endBlock = QByteArray());

    virtual bool isOpen() override;

protected:
    explicit IoDeviceZeraSerial(QObject *parent = nullptr);
    friend class IoDeviceFactory;
private slots:
    void onIoFinished();
    void onDeviceFileGone(QString);
private:
    void _close();
    IoDeviceZeraSerialPrivate *d_ptr;
    Q_DECLARE_PRIVATE(IoDeviceZeraSerial)
};

#endif // IODEVICEZERASERIAL_H
