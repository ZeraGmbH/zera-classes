#include "iotransferdatasingle.h"
#include "iodevicebase.h"

const QByteArray IoTransferDataSingle::demoErrorResponseData = QByteArrayLiteral("_ERROR_RESPONSE_");

IoTransferDataSingle::IoTransferDataSingle()
{
}

IoTransferDataSingle::IoTransferDataSingle(QByteArray bytesSend,
              QByteArray bytesExpectedLead,
              QByteArray bytesExpectedTrail,
              int responseTimeoutMs,
              bool demoErrorResponse) :
    m_demoErrorResponse(demoErrorResponse),
    m_responseTimeoutMs(responseTimeoutMs),
    m_bytesExpectedLead(bytesExpectedLead),
    m_bytesExpectedTrail(bytesExpectedTrail),
    m_bytesSend(bytesSend)
{}

QByteArray IoTransferDataSingle::getDataReceived() const
{
    return m_dataReceived;
}

bool IoTransferDataSingle::didIoPass() const
{
    return m_IoEval == EVAL_PASS;
}

IoTransferDataSingle::EvalResponse IoTransferDataSingle::getEvaluation() const
{
    return m_IoEval;
}

bool IoTransferDataSingle::checkUnusedData() const
{
    bool unused = m_dataReceived.isEmpty() && m_IoEval == EVAL_NOT_EXECUTED;
    if(!unused ) {
        qCritical("Do not reuse IoTransferDataSingle");
    }
    return unused;
}

QByteArray IoTransferDataSingle::getByesSend() const
{
    return m_bytesSend;
}

QByteArray IoTransferDataSingle::getDemoResponse() const
{
    return m_demoErrorResponse ? demoErrorResponseData : m_bytesExpectedLead+m_bytesExpectedTrail;
}

QByteArray IoTransferDataSingle::getExpectedDataLead() const
{
    return m_bytesExpectedLead;
}

QByteArray IoTransferDataSingle::getExpectedDataTrail() const
{
    return m_bytesExpectedTrail;
}

int IoTransferDataSingle::getResponseTimeout() const
{
    return m_responseTimeoutMs;
}

void IoTransferDataSingle::setDataReceived(QByteArray dataReceived)
{
    m_dataReceived = dataReceived;
}

void IoTransferDataSingle::adjustZeroToDefaultTimeouts()
{
    if(m_responseTimeoutMs == 0) {
        m_responseTimeoutMs = ioDefaultTimeout;
    }
}

void IoTransferDataSingle::evaluateResponseLeadTrail()
{
    if(m_dataReceived.isEmpty()) {
        m_IoEval = IoTransferDataSingle::EVAL_NO_ANSWER;
    }
    else {
        bool pass =
                m_dataReceived.startsWith(m_bytesExpectedLead) &&
                m_dataReceived.endsWith(m_bytesExpectedTrail);
        m_IoEval = pass ? IoTransferDataSingle::EVAL_PASS : IoTransferDataSingle::EVAL_WRONG_ANSWER;
    }
}

tIoTransferDataSingleShPtr IoTransferDataSingleFactory::createIoData(QByteArray bytesSend,
                                                                     QByteArray bytesExpectedLead,
                                                                     QByteArray bytesExpectedTrail,
                                                                     int responseTimeoutMs,
                                                                     bool demoErrorResponse)
{
    return tIoTransferDataSingleShPtr(new IoTransferDataSingle(bytesSend,
                                                               bytesExpectedLead,
                                                               bytesExpectedTrail,
                                                               responseTimeoutMs,
                                                               demoErrorResponse));
}
