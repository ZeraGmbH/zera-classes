#ifndef IOTRANSFERDATASINGLE_H
#define IOTRANSFERDATASINGLE_H

#include <QObject>
#include <QByteArray>
#include <QList>
#include <QSharedPointer>

class IoTransferDataSingle
{
public:
    typedef QSharedPointer<IoTransferDataSingle> Ptr;
    IoTransferDataSingle();
    IoTransferDataSingle(
            QByteArray bytesSend,
            QByteArray bytesExpectedLead,
            QByteArray bytesExpectedTrail = "\r",
            int responseTimeoutMs = 0,
            bool demoErrorResponse = false);
    IoTransferDataSingle(
            QByteArray bytesSend,
            QList<QByteArray> bytesExpectedLeadList,
            QByteArray bytesExpectedTrail = "\r",
            int responseTimeoutMs = 0,
            bool demoErrorResponse = false);

    void setDataReceived(QByteArray dataReceived);
    QByteArray getDataReceived() const;

    bool didIoPass() const;
    bool wasNotRunYet() const;
    bool noAnswerReceived() const;
    bool wrongAnswerReceived() const;

    QByteArray getBytesSend() const;
    int getResponseTimeout() const;

    static const QByteArray demoErrorResponseData;
    QByteArray getDemoResponse() const;
    bool m_demoErrorResponse = false;

private:
    QByteArray getExpectedDataLead() const;
    QByteArray getExpectedDataTrail() const;
    void evaluateResponseLeadTrail();

    QByteArray m_dataReceived;
    int m_responseTimeoutMs = 0;
    QList<QByteArray> m_bytesExpectedLeadList;
    QByteArray m_bytesExpectedTrail = "\r";
    QByteArray m_bytesSend;
    enum EvalResponse {
        EVAL_NOT_EXECUTED = 0,
        EVAL_NO_ANSWER,
        EVAL_WRONG_ANSWER,
        EVAL_PASS
    };
    EvalResponse m_IoEval = EVAL_NOT_EXECUTED;
};

#endif // IOTRANSFERDATASINGLE_H
