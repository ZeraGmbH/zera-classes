#include "iodevicebase.h"


IoDeviceBase::IoDeviceBase(IoDeviceTypes type) :
    m_type(type)
{
    connect(this, &IoDeviceBase::_sigIoFinished,
            this, &IoDeviceBase::onIoFinished,
            Qt::QueuedConnection);
}

void IoDeviceBase::onIoFinished(int ioID, bool ioDeviceError)
{
    m_ioTransferData->evaluateResponseLeadTrail();
    emit sigIoFinished(ioID, ioDeviceError);
}

void IoDeviceBase::prepareSendAndReceive(IoTransferDataSingle::Ptr ioTransferData)
{
    m_ioTransferData = ioTransferData;
    setReadTimeoutNextIo(m_ioTransferData->getResponseTimeout());
    m_ioTransferData->checkUnusedData();
    m_currIoId.setPending(m_IDGenerator.nextID());
}

QString IoDeviceBase::getDeviceInfo()
{
    return m_strDeviceInfo;
}
