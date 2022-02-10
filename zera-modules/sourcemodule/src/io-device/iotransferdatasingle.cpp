#include "iotransferdatasingle.h"

IoTransferDataSingle::IoTransferDataSingle(QByteArray bytesSend,
                                           QByteArray bytesExpectedLead,
                                           QByteArray bytesExpectedTrail,
                                           int responseTimeoutMs) :
    m_responseTimeoutMs(responseTimeoutMs),
    m_bytesExpectedTrail(bytesExpectedTrail),
    m_bytesSend(bytesSend),
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
    m_demoResponder(IoTransferDemoResponder::Ptr::create(bytesExpectedLeadList.isEmpty() ? QByteArray() : bytesExpectedLeadList[0], bytesExpectedTrail))
{
    m_bytesExpectedLeadList = bytesExpectedLeadList;
    if(m_bytesExpectedLeadList.isEmpty()) {
        m_bytesExpectedLeadList.append(QByteArray());
    }
}

void IoTransferDataSingle::setDataReceived(QByteArray dataReceived)
{
    m_dataReceived = dataReceived;
    evaluateResponseLeadTrail();
}

QByteArray IoTransferDataSingle::getDataReceived() const
{
    return m_dataReceived;
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
    return m_dataReceived.isEmpty() && m_IoEval == EVAL_NOT_EXECUTED;
}

bool IoTransferDataSingle::noAnswerReceived() const
{
    return m_IoEval == EVAL_NO_ANSWER;
}

bool IoTransferDataSingle::wrongAnswerReceived() const
{
    return m_IoEval == EVAL_WRONG_ANSWER;
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

void IoTransferDataSingle::evaluateResponseLeadTrail()
{
    if(m_dataReceived.isEmpty()) {
        m_IoEval = IoTransferDataSingle::EVAL_NO_ANSWER;
    }
    else {
        bool pass = false;
        for(int idx=0; idx<m_bytesExpectedLeadList.count(); ++idx) {
            QByteArray bytesExpectedLead = m_bytesExpectedLeadList[idx];
            if(m_dataReceived.startsWith(bytesExpectedLead) && m_dataReceived.endsWith(m_bytesExpectedTrail)) {
                pass = true;
                m_passIdxInExpectedLead = idx;
                break;
            }
        }
        m_IoEval = pass ? IoTransferDataSingle::EVAL_PASS : IoTransferDataSingle::EVAL_WRONG_ANSWER;
    }
}
