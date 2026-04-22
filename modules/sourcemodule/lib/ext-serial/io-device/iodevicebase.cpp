#include "iodevicebase.h"

IoDeviceBase::IoDeviceBase(IoDeviceTypes type) :
    m_type(type)
{
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

void IoDeviceBase::emitSigIoFinished(int ioID, bool ioDeviceError)
{
    QMetaObject::invokeMethod(this,
                              "sigIoFinished",
                              Qt::QueuedConnection,
                              Q_ARG(int, ioID),
                              Q_ARG(bool, ioDeviceError));
}

QString IoDeviceBase::getDeviceInfo()
{
    return m_strDeviceInfo;
}
