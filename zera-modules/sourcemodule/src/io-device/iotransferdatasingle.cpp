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


void IoTransferDataSingle::checkUnusedData()
{
    if(!m_dataReceived.isEmpty() || m_IoEval != EVAL_NOT_EXECUTED) {
        qCritical("Do not reuse IoTransferDataSingle");
    }
}

QByteArray IoTransferDataSingle::getDemoResponse()
{
    return m_demoErrorResponse ? demoErrorResponseData : m_bytesExpectedLead+m_bytesExpectedTrail;
}

bool IoTransferDataSingle::evaluateResponse()
{
    bool pass = false;
    if(m_dataReceived.isEmpty()) {
        m_IoEval = IoTransferDataSingle::EVAL_NO_ANSWER;
    }
    else {
        pass =
                m_dataReceived.startsWith(m_bytesExpectedLead) &&
                m_dataReceived.endsWith(m_bytesExpectedTrail);
        m_IoEval = pass ? IoTransferDataSingle::EVAL_PASS : IoTransferDataSingle::EVAL_WRONG_ANSWER;
    }
    return pass;
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
