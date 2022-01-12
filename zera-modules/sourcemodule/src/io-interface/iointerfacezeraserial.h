#ifndef IOINTERFACEZERASERIAL_H
#define IOINTERFACEZERASERIAL_H

#include <QObject>
#include "iointerfacebase.h"

class IoInterfaceZeraSerialPrivate;
class IoInterfaceZeraSerial : public IoInterfaceBase
{
    Q_OBJECT
public:
    virtual ~IoInterfaceZeraSerial();

    virtual IoInterfaceTypes type() override { return SOURCE_INTERFACE_ASYNCSERIAL; }
    virtual bool open(QString strDeviceInfo) override; // e.g "/dev/ttyUSB0"
    virtual void close() override;
    virtual void setReadTimeoutNextIo(int timeoutMs) override;
    virtual int sendAndReceive(QByteArray bytesSend, QByteArray* pDataReceive) override;

    // wrappers - see https://github.com/ZeraGmbH/qtiohelper / QT += serialportasyncblock
    void setReadTimeoutNextIo(int iMsReceiveFirst, int iMsBetweenTwoBytes, int iMsMinTotal = 0);
    void setBlockEndCriteriaNextIo(int iBlockLenReceive = 0, QByteArray endBlock = QByteArray());

    virtual bool isOpen() override;

protected:
    explicit IoInterfaceZeraSerial(QObject *parent = nullptr);
    friend class IoInterfaceFactory;
private slots:
    void onIoFinished();
    void onDeviceFileGone(QString);
private:
    void _close();
    IoInterfaceZeraSerialPrivate *d_ptr;
    Q_DECLARE_PRIVATE(IoInterfaceZeraSerial)
};

#endif // IOINTERFACEZERASERIAL_H
