#ifndef IOSERIALDEVICEBASE_H
#define IOSERIALDEVICEBASE_H

#include "iodevicefactory.h"
#include "io-ids/ioidgenerator.h"
#include "io-ids/ioidkeeper.h"

#include <QObject>

static constexpr int sourceDefaultTimeout = 1500;

class IODeviceBaseSerial : public QObject
{
    Q_OBJECT
public:
    virtual ~IODeviceBaseSerial();

    virtual bool open(QString) { return false; }
    virtual void close() {}
    virtual void setReadTimeoutNextIo(int) {};
    virtual int sendAndReceive(QByteArray bytesSend, QByteArray* pDataReceive);
    virtual void simulateExternalDisconnect() {}

    virtual bool isOpen() { return false; }
    virtual IoDeviceTypes type() { return SERIAL_DEVICE_BASE; }

    QString getDeviceInfo();

    bool isDemo() { return type() == SERIAL_DEVICE_DEMO; }

signals:
    void sigDisconnected();
    void sigIoFinished(int ioID, bool ioDeviceError); // users connect this signal
    void sigIoFinishedToQueue(int ioID, bool ioDeviceError); // sub classes emit this to ensure queue

protected:
    explicit IODeviceBaseSerial(QObject *parent = nullptr);
    friend class IoDeviceFactory;

    QString m_strDeviceInfo;
    IoIdGenerator m_IDGenerator;
    IoIdKeeper m_currIoId;
};

#endif // IOSERIALDEVICEBASE_H
