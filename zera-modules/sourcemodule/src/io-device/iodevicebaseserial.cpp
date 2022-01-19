#include "iodevicebaseserial.h"

IODeviceBaseSerial::IODeviceBaseSerial(IoDeviceTypes type) :
    m_type(type)
{
    connect(this, &IODeviceBaseSerial::sigIoFinishedToQueue, this, &IODeviceBaseSerial::sigIoFinished, Qt::QueuedConnection);
}

void IODeviceBaseSerial::prepareSendAndReceive(tIoTransferDataSingleShPtr ioTransferData)
{
    m_ioTransferData = ioTransferData;
    m_ioTransferData->checkUnusedData();
    m_currIoId.setCurrent(m_IDGenerator.nextID());
}

int IODeviceBaseSerial::sendAndReceive(tIoTransferDataSingleShPtr ioTransferData)
{
    prepareSendAndReceive(ioTransferData);
    emit sigIoFinishedToQueue(m_currIoId.getCurrent(), true);
    return m_currIoId.getCurrent();
}

QString IODeviceBaseSerial::getDeviceInfo()
{
    return m_strDeviceInfo;
}
