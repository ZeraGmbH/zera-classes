#include "iodevicebase.h"

IoDeviceBase::IoDeviceBase(IoDeviceTypes type) :
    m_type(type)
{
    connect(this, &IoDeviceBase::sigIoFinishedQueued,
            this, &IoDeviceBase::sigIoFinished,
            Qt::QueuedConnection);
}

void IoDeviceBase::prepareSendAndReceive(IoTransferDataSingle::Ptr ioTransferData)
{
    m_ioTransferData = ioTransferData;
    setReadTimeoutNextIo(m_ioTransferData->getResponseTimeout());
    if(!m_ioTransferData->wasNotRunYet()) {
        qCritical("Do not reuse IoTransferDataSingle");
    }
    m_currIoId.setPending(m_IDGenerator.nextID());
}

QString IoDeviceBase::getDeviceInfo()
{
    return m_strDeviceInfo;
}
