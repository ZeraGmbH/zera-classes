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

void IoDeviceBase::prepareSendAndReceive(tIoTransferDataSingleShPtr ioTransferData)
{
    m_ioTransferData = ioTransferData;
    m_ioTransferData->checkUnusedData();
    m_currIoId.setCurrent(m_IDGenerator.nextID());
}

QString IoDeviceBase::getDeviceInfo()
{
    return m_strDeviceInfo;
}
