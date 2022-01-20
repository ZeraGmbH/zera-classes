#include "iotransferdatasingle.h"

const QByteArray IoTransferDataSingle::demoErrorResponseData = QByteArrayLiteral("foo");

IoTransferDataSingle::IoTransferDataSingle()
{
}

IoTransferDataSingle::IoTransferDataSingle(QByteArray bytesSend,
              QByteArray bytesExpectedLead,
              QByteArray bytesExpectedTrail,
              int responseTimeoutMs,
              bool demoErrorResponse) :
    m_responseTimeoutMs(responseTimeoutMs),
    m_bytesSend(bytesSend),
    m_bytesExpectedLead(bytesExpectedLead),
    m_bytesExpectedTrail(bytesExpectedTrail),
    m_demoErrorResponse(demoErrorResponse)
{}

bool IoTransferDataSingle::didIoPass()
{
    return m_IoEval == EVAL_PASS;
}

IoTransferDataSingle::EvalResponse IoTransferDataSingle::getEvaluation()
{
    return m_IoEval;
}

bool IoTransferDataSingle::checkUnusedData()
{
    bool unused = m_dataReceived.isEmpty() && m_IoEval == EVAL_NOT_EXECUTED;
    if(!unused ) {
        qCritical("Do not reuse IoTransferDataSingle");
    }
    return unused;
}

QByteArray IoTransferDataSingle::getDemoResponse()
{
    return m_demoErrorResponse ? demoErrorResponseData : m_bytesExpectedLead+m_bytesExpectedTrail;
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
