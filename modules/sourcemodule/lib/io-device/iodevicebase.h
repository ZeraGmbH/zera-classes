#ifndef IODEVICEBASE_H
#define IODEVICEBASE_H

#include "iodevicetypes.h"
#include "iotransferdatasingle.h"
#include "idgenerator.h"
#include "idkeeper.h"

#include <QObject>

class IoDeviceBase : public QObject
{
    Q_OBJECT
public:
    typedef QSharedPointer<IoDeviceBase> Ptr;
    IoDeviceBase(IoDeviceTypes type);
    virtual bool open(QString) = 0;
    virtual void close() = 0;
    virtual bool isOpen() = 0;
    virtual int sendAndReceive(IoTransferDataSingle::Ptr ioTransferData) = 0;
    QString getDeviceInfo();
    IoDeviceTypes getType() { return m_type; }
signals:
    void sigDisconnected();
    void sigIoFinished(int ioID, bool ioDeviceError); // users connect this signal

protected:
    virtual void setReadTimeoutNextIo(int) {};
    void prepareSendAndReceive(IoTransferDataSingle::Ptr ioTransferData);
    QString m_strDeviceInfo;
    IoIdGenerator m_IDGenerator;
    IdKeeperSingle m_currIoId;
    IoTransferDataSingle::Ptr m_ioTransferData;

private:
    IoDeviceTypes m_type;
signals: // sub classes emit this to ensure queue
    void sigIoFinishedQueued(int ioID, bool ioDeviceError);
};

#endif // IODEVICEBASE_H
