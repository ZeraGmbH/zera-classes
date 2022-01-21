#ifndef IODEVICEBROKENDUMMY_H
#define IODEVICEBROKENDUMMY_H

#include "iodevicebase.h"

#include <QObject>

class IoDeviceBrokenDummy : public IoDeviceBase
{
    Q_OBJECT
public:
    virtual bool open(QString) override { return false; }
    virtual void close() override {}
    virtual bool isOpen() override { return false; }

    virtual int sendAndReceive(IoTransferDataSingle::Ptr ioTransferData) override;

protected:
    IoDeviceBrokenDummy(IoDeviceTypes type);
    friend class IoDeviceFactory;
};

#endif // IODEVICEBROKENDUMMY_H
