#ifndef IODEVICEBASE_H
#define IODEVICEBASE_H

#include "iodevicefactory.h"
#include "iotransferdatasingle.h"
#include "io-ids/ioidgenerator.h"
#include "io-ids/ioidkeeper.h"

#include <QObject>

static constexpr int sourceDefaultTimeout = 1500;

class IoDeviceBase : public QObject
{
    Q_OBJECT
public:
    virtual bool open(QString) = 0;
    virtual void close() = 0;

    virtual bool isOpen() = 0;

    virtual void setReadTimeoutNextIo(int) {};
    virtual int sendAndReceive(tIoTransferDataSingleShPtr ioTransferData) = 0;

    virtual void simulateExternalDisconnect() {}

    QString getDeviceInfo();
    IoDeviceTypes type() { return m_type; }
    bool isDemo() { return type() == SERIAL_DEVICE_DEMO; }

signals:
    void sigDisconnected();
    void sigIoFinished(int ioID, bool ioDeviceError); // users connect this signal

protected: signals: // Just to state out: sub classes emit this to ensure evaluation & queue
    void _sigIoFinished(int ioID, bool ioDeviceError);
protected:
    friend class IoDeviceFactory;
    IoDeviceBase(IoDeviceTypes type);
    void prepareSendAndReceive(tIoTransferDataSingleShPtr ioTransferData);

    QString m_strDeviceInfo;
    IoIdGenerator m_IDGenerator;
    IoIdKeeper m_currIoId;
    tIoTransferDataSingleShPtr m_ioTransferData;

private:
    IoDeviceTypes m_type;
private slots:
    void onIoFinished(int ioID, bool ioDeviceError);
};

#endif // IODEVICEBASE_H
