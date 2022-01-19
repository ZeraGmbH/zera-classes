#ifndef IOSERIALDEVICEBASE_H
#define IOSERIALDEVICEBASE_H

#include "iodevicefactory.h"
#include "iotransferdatasingle.h"
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

    virtual int sendAndReceive(tIoTransferDataSingleShPtr ioTransferData);
    virtual void simulateExternalDisconnect() {}

    virtual bool isOpen() { return false; }

    QString getDeviceInfo();
    IoDeviceTypes type() { return m_type; }
    bool isDemo() { return type() == SERIAL_DEVICE_DEMO; }

signals:
    void sigDisconnected();
    void sigIoFinished(int ioID, bool ioDeviceError); // users connect this signal
    void sigIoFinishedToQueue(int ioID, bool ioDeviceError); // sub classes emit this to ensure queue

protected:
    explicit IODeviceBaseSerial(IoDeviceTypes type);
    void prepareSendAndReceive(tIoTransferDataSingleShPtr ioTransferData);

    friend class IoDeviceFactory;

    IoDeviceTypes m_type;
    QString m_strDeviceInfo;
    IoIdGenerator m_IDGenerator;
    IoIdKeeper m_currIoId;
    tIoTransferDataSingleShPtr m_ioTransferData;
};

#endif // IOSERIALDEVICEBASE_H
