#ifndef IOTRANSFERDATASINGLE_H
#define IOTRANSFERDATASINGLE_H

#include "iotransferdemoresponder.h"
#include "ioquerycontentevaluator.h"
#include <QObject>
#include <QByteArray>
#include <QList>
#include <QSharedPointer>

class IoTransferDataSingle
{
public:
    typedef QSharedPointer<IoTransferDataSingle> Ptr;
    IoTransferDataSingle(QByteArray bytesSend,
                         QByteArray bytesExpectedLead,
                         QByteArray bytesExpectedTrail = "\r",
                         int responseTimeoutMs = 0);
    IoTransferDataSingle(QByteArray bytesSend,
                         QList<QByteArray> bytesExpectedLeadList,
                         QByteArray bytesExpectedTrail = "\r",
                         int responseTimeoutMs = 0);
    void setDataReceived(QByteArray dataReceived);
    void setCustomQueryContentEvaluator(IIoQueryContentEvaluator::Ptr evaluator);
    QByteArray getBytesReceived() const;
    QByteArray getBytesQueryContent() const;
    bool hasCustomQueryEvaluator() const;
    bool didIoPass() const;
    int getPassIdxInExpectedLead() const;
    bool wasNotRunYet() const;
    bool noAnswerReceived() const;
    bool wrongAnswerReceived() const;
    bool queryContentFailed() const;
    QByteArray getBytesSend() const;
    int getResponseTimeout() const;
    IoTransferDemoResponder::Ptr getDemoResponder();

private:
    QByteArray getExpectedResponseLead() const;
    QByteArray getExpectedResponseTrail() const;
    void evaluateResponse();
    bool evaluateResponseEmpty();
    bool evaluateResponseLeadTrail();
    bool evaluateResponseQueryContent();
    QByteArray m_bytesReceived;
    QByteArray m_queryContent;
    int m_responseTimeoutMs = 0;
    QList<QByteArray> m_bytesExpectedLeadList;
    QByteArray m_bytesExpectedTrail = "\r";
    QByteArray m_bytesSend;
    enum EvalResponse {
        EVAL_NOT_EXECUTED = 0,
        EVAL_NO_ANSWER,
        EVAL_WRONG_ANSWER,
        EVAL_QUERY_CONTENT_FAIL,
        EVAL_PASS
    };
    EvalResponse m_IoEval = EVAL_NOT_EXECUTED;
    IIoQueryContentEvaluator::Ptr m_queryContentEvaluator;
    bool m_hasCustomQueryEvaluator = false;
    int m_passIdxInExpectedLead = -1;
    IoTransferDemoResponder::Ptr m_demoResponder;
};

#endif // IOTRANSFERDATASINGLE_H
