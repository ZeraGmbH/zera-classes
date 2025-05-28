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
    virtual bool open(QString strDeviceInfo) = 0;
    virtual void close() = 0;
    virtual bool isOpen() = 0;
    virtual int sendAndReceive(IoTransferDataSingle::Ptr ioTransferData) = 0;
    QString getDeviceInfo();
    IoDeviceTypes getType() { return m_type; }
signals:
    void sigDisconnected();
    void sigIoFinished(int ioID, bool ioDeviceError);

protected:
    virtual void setReadTimeoutNextIo(int) {};
    void prepareSendAndReceive(IoTransferDataSingle::Ptr ioTransferData);
    void emitSigIoFinished(int ioID, bool ioDeviceError);
    QString m_strDeviceInfo;
    IoIdGenerator m_IDGenerator;
    IdKeeperSingle m_currIoId;
    IoTransferDataSingle::Ptr m_ioTransferData;

private:
    IoDeviceTypes m_type;
};

#endif // IODEVICEBASE_H
