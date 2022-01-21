#include "iodevicebrokendummy.h"

IoDeviceBrokenDummy::IoDeviceBrokenDummy(IoDeviceTypes type) :
    IoDeviceBase(type)
{
}

int IoDeviceBrokenDummy::sendAndReceive(IoTransferDataSingle::Ptr ioTransferData)
{
    prepareSendAndReceive(ioTransferData);
    emit _sigIoFinished(m_currIoId.getCurrent(), true);
    return m_currIoId.getCurrent();
}

