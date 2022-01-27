#include "iotransferdatasingle.h"

IoTransferDataSingle::IoTransferDataSingle(QByteArray bytesSend,
                                           QByteArray bytesExpectedLead,
                                           QByteArray bytesExpectedTrail,
                                           int responseTimeoutMs) :
    m_responseTimeoutMs(responseTimeoutMs),
    m_bytesExpectedTrail(bytesExpectedTrail),
    m_bytesSend(bytesSend),
    m_demoResponder(bytesExpectedLead, bytesExpectedTrail)
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
    m_demoResponder(bytesExpectedLeadList.isEmpty() ? QByteArray() : bytesExpectedLeadList[0], bytesExpectedTrail)
{
    m_bytesExpectedLeadList = bytesExpectedLeadList;
    if(m_bytesExpectedLeadList.isEmpty()) {
        m_bytesExpectedLeadList.append(QByteArray());
    }
}

QByteArray IoTransferDataSingle::getDataReceived() const
{
    return m_dataReceived;
}

bool IoTransferDataSingle::didIoPass() const
{
    return m_IoEval == EVAL_PASS;
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

QByteArray IoTransferDataSingle::getBytesSend() const
{
    return m_bytesSend;
}

QByteArray IoTransferDataSingle::getExpectedDataLead() const
{
    return m_bytesExpectedLeadList[0];
}

QByteArray IoTransferDataSingle::getExpectedDataTrail() const
{
    return m_bytesExpectedTrail;
}

int IoTransferDataSingle::getResponseTimeout() const
{
    return m_responseTimeoutMs;
}

IoTransferDemoResponder &IoTransferDataSingle::getDemoResponder()
{
    return m_demoResponder;
}

void IoTransferDataSingle::setDataReceived(QByteArray dataReceived)
{
    m_dataReceived = dataReceived;
    evaluateResponseLeadTrail();
}

void IoTransferDataSingle::evaluateResponseLeadTrail()
{
    if(m_dataReceived.isEmpty()) {
        m_IoEval = IoTransferDataSingle::EVAL_NO_ANSWER;
    }
    else {
        bool pass = false;
        for(auto bytesExpectedLead : m_bytesExpectedLeadList) {
            if(m_dataReceived.startsWith(bytesExpectedLead) && m_dataReceived.endsWith(m_bytesExpectedTrail)) {
                pass = true;
                break;
            }
        }
        m_IoEval = pass ? IoTransferDataSingle::EVAL_PASS : IoTransferDataSingle::EVAL_WRONG_ANSWER;
    }
}
