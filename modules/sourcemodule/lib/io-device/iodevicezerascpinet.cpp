#include "iodevicezerascpinet.h"

IoDeviceZeraSCPINet::IoDeviceZeraSCPINet(Zera::PcbInterfacePtr pcbInterface) :
    IoDeviceBase(IoDeviceTypes::SCPI_NET),
    m_pcbInterface(pcbInterface)
{
}

bool IoDeviceZeraSCPINet::open(QString strDeviceInfo)
{
    Q_UNUSED(strDeviceInfo)
    return true;
}

void IoDeviceZeraSCPINet::close()
{
}

bool IoDeviceZeraSCPINet::isOpen()
{
    return true;
}

int IoDeviceZeraSCPINet::sendAndReceive(IoTransferDataSingle::Ptr ioTransferData)
{

}
