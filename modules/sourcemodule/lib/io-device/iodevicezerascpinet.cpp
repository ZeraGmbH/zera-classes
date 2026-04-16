#include "iodevicezerascpinet.h"
#include <reply.h>
#include <timerfactoryqt.h>

IoDeviceZeraSCPINet::IoDeviceZeraSCPINet(Zera::PcbInterfacePtr pcbInterface) :
    IoDeviceBase(IoDeviceTypes::SCPI_NET),
    m_pcbInterface(pcbInterface)
{
}

bool IoDeviceZeraSCPINet::open(QString strDeviceInfo)
{
    m_strDeviceInfo = strDeviceInfo;
    connect(m_pcbInterface.get(), &AbstractServerInterface::serverAnswer,
            this, &IoDeviceZeraSCPINet::catchInterfaceAnswer);
    m_isOpen = true;
    return m_isOpen;
}

void IoDeviceZeraSCPINet::close()
{
    disconnect(m_pcbInterface.get(), &AbstractServerInterface::serverAnswer,
               this, &IoDeviceZeraSCPINet::catchInterfaceAnswer);
    m_isOpen = false;
}

bool IoDeviceZeraSCPINet::isOpen()
{
    return m_isOpen;
}

int IoDeviceZeraSCPINet::sendAndReceive(IoTransferDataSingle::Ptr ioTransferData)
{
    if (!m_isOpen) {
        qCritical("Device not open");
        return 0;
    }
    prepareSendAndReceive(ioTransferData);

    m_timeoutTimer = TimerFactoryQt::createSingleShot(m_timeoutMs);
    connect(m_timeoutTimer.get(), &TimerTemplateQt::sigExpired,
            this, &IoDeviceZeraSCPINet::onTimeout);
    m_timeoutTimer->start();

    int id = m_pcbInterface->scpiCommand(ioTransferData->getBytesSend());
    m_currIoId.setPending(id);

    return m_currIoId.getPending();
}

void IoDeviceZeraSCPINet::catchInterfaceAnswer(quint32 msgnr, quint8 reply, const QVariant &answer)
{
    if (!m_ioTransferData->wasNotRunYet())
        return;
    if (m_currIoId.getPending() != int(msgnr))
        return;

    if (reply == ack) {
        m_ioTransferData->setDataReceived(answer.toByteArray());
        emitSigIoFinished(m_currIoId.getPending(), false);
    }
    else
        errorOut(IoTransferDataSingle::EVAL_WRONG_ANSWER, answer.toByteArray());
}

void IoDeviceZeraSCPINet::onTimeout()
{
    if (!m_ioTransferData->wasNotRunYet())
        return;
    errorOut(IoTransferDataSingle::EVAL_NO_ANSWER, "");
}

void IoDeviceZeraSCPINet::setReadTimeoutNextIo(int timeoutMs)
{
    m_timeoutMs = timeoutMs != 0 ? timeoutMs : TRANSACTION_TIMEOUT;
}

void IoDeviceZeraSCPINet::errorOut(IoTransferDataSingle::EvalResponse eval, const QByteArray &response)
{
    m_ioTransferData->setDataReceived(response);
    m_ioTransferData->overrideEvaluation(eval);
    emitSigIoFinished(m_currIoId.getPending(), true);
}
