#include "iodevicebrokendummy.h"

IoDeviceBrokenDummy::IoDeviceBrokenDummy(IoDeviceTypes type) :
    IoDeviceBase(type)
{
}

int IoDeviceBrokenDummy::sendAndReceive(tIoTransferDataSingleShPtr ioTransferData)
{
    prepareSendAndReceive(ioTransferData);
    emit _sigIoFinished(m_currIoId.getCurrent(), true);
    return m_currIoId.getCurrent();
}

