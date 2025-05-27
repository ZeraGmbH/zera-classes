#ifndef IODEVICEZERASCPINET_H
#define IODEVICEZERASCPINET_H

#include "iodevicebase.h"
#include <pcbinterface.h>

class IoDeviceZeraSCPINet : public IoDeviceBase
{
    Q_OBJECT
public:
    IoDeviceZeraSCPINet(Zera::PcbInterfacePtr pcbInterface);
    bool open(QString strDeviceInfo) override;
    void close() override;
    bool isOpen() override;
    int sendAndReceive(IoTransferDataSingle::Ptr ioTransferData) override;

private:
    Zera::PcbInterfacePtr m_pcbInterface;
};

#endif // IODEVICEZERASCPINET_H
