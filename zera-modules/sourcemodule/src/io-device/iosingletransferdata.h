#ifndef IOSINGLETRANSFERDATA_H
#define IOSINGLETRANSFERDATA_H

#include <QObject>
#include <QByteArray>
#include <QList>

class IOSingleTransferData
{
public:
    IOSingleTransferData();
    IOSingleTransferData(QByteArray bytesSend,
                  QByteArray bytesExpectedLead,
                  QByteArray bytesExpectedTrail = "\r",
                  int responseTimeoutMs = 0) :
        m_responseTimeoutMs(responseTimeoutMs),
        m_bytesSend(bytesSend),
        m_bytesExpectedLead(bytesExpectedLead),
        m_bytesExpectedTrail(bytesExpectedTrail)
    {}
    bool operator == (const IOSingleTransferData& other);

    QByteArray m_dataReceived;
    enum {
        EVAL_NOT_EXECUTED = 0,
        EVAL_NO_ANSWER,
        EVAL_WRONG_ANSWER,
        EVAL_PASS
    } m_IoEval = EVAL_NOT_EXECUTED;

    int m_responseTimeoutMs = 0;
    QByteArray m_bytesSend;
    QByteArray m_bytesExpectedLead;
    QByteArray m_bytesExpectedTrail;
};

typedef QList<IOSingleTransferData> tIoTransferList;



#endif // IOSINGLETRANSFERDATA_H
