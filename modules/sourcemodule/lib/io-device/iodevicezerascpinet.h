#ifndef IODEVICEZERASCPINET_H
#define IODEVICEZERASCPINET_H

#include "iodevicebase.h"

class IoDeviceZeraSCPINet : public IoDeviceBase
{
    Q_OBJECT
public:
    IoDeviceZeraSCPINet();
    bool open(QString strDeviceInfo) override;
    void close() override;
    bool isOpen() override;
    int sendAndReceive(IoTransferDataSingle::Ptr ioTransferData) override;
};

#endif // IODEVICEZERASCPINET_H
