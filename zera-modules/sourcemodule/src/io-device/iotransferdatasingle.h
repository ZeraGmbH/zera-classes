#ifndef IOTRANSFERDATASINGLE_H
#define IOTRANSFERDATASINGLE_H

#include <QObject>
#include <QByteArray>
#include <QList>
#include <QSharedPointer>

class IoTransferDataSingle
{
public:
    IoTransferDataSingle();
    IoTransferDataSingle(
            QByteArray bytesSend,
            QByteArray bytesExpectedLead,
            QByteArray bytesExpectedTrail = "\r",
            int responseTimeoutMs = 0,
            bool demoErrorResponse = false);

    enum EvalResponse {
        EVAL_NOT_EXECUTED = 0,
        EVAL_NO_ANSWER,
        EVAL_WRONG_ANSWER,
        EVAL_PASS
    };

    QByteArray getDataReceived() const;
    bool didIoPass() const;
    EvalResponse getEvaluation() const;
    bool checkUnusedData() const;
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
    EvalResponse m_IoEval = EVAL_NOT_EXECUTED;
};

typedef QSharedPointer<IoTransferDataSingle> tIoTransferDataSingleShPtr;

class IoTransferDataSingleFactory
{
public:
    static tIoTransferDataSingleShPtr createIoData(
            QByteArray bytesSend,
            QByteArray bytesExpectedLead,
            QByteArray bytesExpectedTrail = "\r",
            int responseTimeoutMs = 0,
            bool demoErrorResponse = false);
};

#endif // IOTRANSFERDATASINGLE_H
