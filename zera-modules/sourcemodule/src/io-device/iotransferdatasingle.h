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

    QByteArray getDataReceived() const;
    bool didIoPass() const;
    bool wasNotRunYet() const;
    bool noAnswerReceived() const;
    bool wrongAnswerReceived() const;

    QByteArray getByesSend() const;
    QByteArray getDemoResponse() const;
    QByteArray getExpectedDataLead() const;
    QByteArray getExpectedDataTrail() const;
    int getResponseTimeout() const;

    void setDataReceived(QByteArray dataReceived);
    void adjustZeroToDefaultTimeouts();
    void evaluateResponseLeadTrail();

    static const QByteArray demoErrorResponseData;

    bool m_demoErrorResponse = false;

protected:
    QByteArray m_dataReceived;
    int m_responseTimeoutMs = 0;
    QByteArray m_bytesExpectedLead;
    QByteArray m_bytesExpectedTrail = "\r";
    QByteArray m_bytesSend;
private:
    enum EvalResponse {
        EVAL_NOT_EXECUTED = 0,
        EVAL_NO_ANSWER,
        EVAL_WRONG_ANSWER,
        EVAL_PASS
    };
    EvalResponse m_IoEval = EVAL_NOT_EXECUTED;
};

#endif // IOTRANSFERDATASINGLE_H
