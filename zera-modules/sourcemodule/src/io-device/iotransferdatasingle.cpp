#include "iotransferdatasingle.h"

IoTransferDataSingle::IoTransferDataSingle(QByteArray bytesSend,
                                           QByteArray bytesExpectedLead,
                                           QByteArray bytesExpectedTrail,
                                           int responseTimeoutMs) :
    m_responseTimeoutMs(responseTimeoutMs),
    m_bytesExpectedTrail(bytesExpectedTrail),
    m_bytesSend(bytesSend),
    m_queryContentEvaluator(IIoQueryContentEvaluator::Ptr(new IoQueryContentEvaluatorAlwaysPass)),
    m_demoResponder(IoTransferDemoResponder::Ptr::create(bytesExpectedLead, bytesExpectedTrail))
{
    m_bytesExpectedLeadList.append(bytesExpectedLead);
}

IoTransferDataSingle::IoTransferDataSingle(QByteArray bytesSend,
                                           QList<QByteArray> bytesExpectedLeadList,
                                           QByteArray bytesExpectedTrail,
                                           int responseTimeoutMs) :
    m_responseTimeoutMs(responseTimeoutMs),
    m_bytesExpectedTrail(bytesExpectedTrail),
    m_bytesSend(bytesSend),
    m_queryContentEvaluator(IIoQueryContentEvaluator::Ptr(new IoQueryContentEvaluatorAlwaysPass)),
    m_demoResponder(IoTransferDemoResponder::Ptr::create(bytesExpectedLeadList.isEmpty() ? QByteArray() : bytesExpectedLeadList[0], bytesExpectedTrail))
{
    m_bytesExpectedLeadList = bytesExpectedLeadList;
    if(m_bytesExpectedLeadList.isEmpty()) {
        m_bytesExpectedLeadList.append(QByteArray());
    }
}

void IoTransferDataSingle::setDataReceived(QByteArray dataReceived)
{
    m_bytesReceived = dataReceived;
    evaluateResponse();
}

void IoTransferDataSingle::setCustomQueryContentEvaluator(IIoQueryContentEvaluator::Ptr evaluator)
{
    m_queryContentEvaluator = evaluator;
    m_hasCustomQueryEvaluator = true;
}

QByteArray IoTransferDataSingle::getBytesReceived() const
{
    return m_bytesReceived;
}

QByteArray IoTransferDataSingle::getBytesQueryContent() const
{
    return m_queryContent;
}

bool IoTransferDataSingle::hasCustomQueryEvaluator() const
{
    return m_hasCustomQueryEvaluator;
}

bool IoTransferDataSingle::didIoPass() const
{
    return m_IoEval == EVAL_PASS;
}

int IoTransferDataSingle::getPassIdxInExpectedLead() const
{
    return m_passIdxInExpectedLead;
}

bool IoTransferDataSingle::wasNotRunYet() const
{
    return m_bytesReceived.isEmpty() && m_IoEval == EVAL_NOT_EXECUTED;
}

bool IoTransferDataSingle::noAnswerReceived() const
{
    return m_IoEval == EVAL_NO_ANSWER;
}

bool IoTransferDataSingle::wrongAnswerReceived() const
{
    return m_IoEval == EVAL_WRONG_ANSWER;
}

bool IoTransferDataSingle::queryContentFailed() const
{
    return m_IoEval == EVAL_QUERY_CONTENT_FAIL;
}

int IoTransferDataSingle::getResponseTimeout() const
{
    return m_responseTimeoutMs;
}

QByteArray IoTransferDataSingle::getBytesSend() const
{
    return m_bytesSend;
}

QByteArray IoTransferDataSingle::getExpectedResponseLead() const
{
    return m_bytesExpectedLeadList[0];
}

QByteArray IoTransferDataSingle::getExpectedResponseTrail() const
{
    return m_bytesExpectedTrail;
}

IoTransferDemoResponder::Ptr IoTransferDataSingle::getDemoResponder()
{
    return m_demoResponder;
}

void IoTransferDataSingle::evaluateResponse()
{
    if(evaluateResponseEmpty() &&
            evaluateResponseLeadTrail() &&
            evaluateResponseQueryContent()) {
        m_IoEval = IoTransferDataSingle::EVAL_PASS;
    }
}

bool IoTransferDataSingle::evaluateResponseEmpty()
{
    // on protocols other than serial we have to rethink this
    bool pass = true;
    if(m_bytesReceived.isEmpty()) {
        pass = false;
        m_IoEval = IoTransferDataSingle::EVAL_NO_ANSWER;
    }
    return pass;
}

bool IoTransferDataSingle::evaluateResponseLeadTrail()
{
    bool pass = false;
    for(int idx=0; idx<m_bytesExpectedLeadList.count(); ++idx) {
        QByteArray bytesExpectedLead = m_bytesExpectedLeadList[idx];
        if(m_bytesReceived.startsWith(bytesExpectedLead) && m_bytesReceived.endsWith(m_bytesExpectedTrail)) {
            pass = true;
            m_passIdxInExpectedLead = idx;
            int queryContentLength =
                    m_bytesReceived.length() - bytesExpectedLead.length() - m_bytesExpectedTrail.length();
            m_queryContent = m_bytesReceived.mid(bytesExpectedLead.length(), queryContentLength);
            break;
        }
    }
    if(!pass) {
        m_IoEval = IoTransferDataSingle::EVAL_WRONG_ANSWER;
    }
    return pass;
}

bool IoTransferDataSingle::evaluateResponseQueryContent()
{
    bool pass = m_queryContentEvaluator->evalQueryResponse(m_queryContent);
    if(!pass) {
        m_IoEval = IoTransferDataSingle::EVAL_QUERY_CONTENT_FAIL;
    }
    return pass;
}
