#ifndef IODEVICEBASE_H
#define IODEVICEBASE_H

#include "iodevicetypes.h"
#include "iotransferdatasingle.h"
#include "io-ids/ioidgenerator.h"
#include "io-ids/ioidkeeper.h"

#include <QObject>

static constexpr int ioDefaultTimeout = 1500;

class IoDeviceBase : public QObject
{
    Q_OBJECT
public:
    typedef QSharedPointer<IoDeviceBase> Ptr;

    virtual bool open(QString) = 0;
    virtual void close() = 0;

    virtual bool isOpen() = 0;

    virtual int sendAndReceive(IoTransferDataSingle::Ptr ioTransferData) = 0;

    virtual void simulateExternalDisconnect() {}

    QString getDeviceInfo();
    IoDeviceTypes getType() { return m_type; }
    bool isDemo() { return IoDeviceTypeQuery::isDemo(m_type); }

signals:
    void sigDisconnected();
    void sigIoFinished(int ioID, bool ioDeviceError); // users connect this signal

protected: signals: // Just to state out: sub classes emit this to ensure evaluation & queue
    void _sigIoFinished(int ioID, bool ioDeviceError);
protected:
    friend class IoDeviceFactory;
    IoDeviceBase(IoDeviceTypes type);
    virtual void setReadTimeoutNextIo(int) {};
    void prepareSendAndReceive(IoTransferDataSingle::Ptr ioTransferData);

    QString m_strDeviceInfo;
    IoIdGenerator m_IDGenerator;
    IoIdKeeper m_currIoId;
    IoTransferDataSingle::Ptr m_ioTransferData;

private:
    IoDeviceTypes m_type;
private slots:
    void onIoFinished(int ioID, bool ioDeviceError);
};

#endif // IODEVICEBASE_H
