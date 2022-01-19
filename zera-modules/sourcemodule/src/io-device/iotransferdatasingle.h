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

    bool operator == (const IoTransferDataSingle& other);
    void checkUnusedData();
    QByteArray getDemoResponse();
    bool evaluateResponse();

    static const QByteArray demoErrorResponseData;
    QByteArray m_dataReceived;
    enum EvalResponse {
        EVAL_NOT_EXECUTED = 0,
        EVAL_NO_ANSWER,
        EVAL_WRONG_ANSWER,
        EVAL_PASS
    };
    EvalResponse m_IoEval = EVAL_NOT_EXECUTED;

    int m_responseTimeoutMs = 0;
    QByteArray m_bytesSend;
    QByteArray m_bytesExpectedLead;
    QByteArray m_bytesExpectedTrail = "\r";
    bool m_demoErrorResponse = false;
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
