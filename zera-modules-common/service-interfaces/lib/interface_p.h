#ifndef INTERFACE_P_H
#define INTERFACE_P_H

#include <proxyclient.h>
#include <netmessages.pb.h>
#include <QObject>
#include <QAbstractSocket>
#include <QHash>
#include <QVariant>

namespace Zera {

class cInterfacePrivate: public QObject
{
    Q_OBJECT
protected:
    struct TAnswerDecoded
    {
        quint32 msgNr;
        QString msgBody;
        int reply;
        int cmdSendEnumVal;
    };
    bool decodeProtobuffAnswer(std::shared_ptr<ProtobufMessage::NetMessage> message,
                               TAnswerDecoded &decodedAnswer,
                               int interruptEnumVal = -1);
    quint32 sendCommand(QString cmd);
    quint32 sendCommand(QString cmd, QString par);

    QHash<quint32, int> m_MsgNrCmdList;
    Zera::ProxyClient *m_pClient;
protected slots:
    virtual void receiveAnswer(std::shared_ptr<ProtobufMessage::NetMessage> message) = 0;
    virtual void receiveError(QAbstractSocket::SocketError errorCode) = 0;
};

}

#endif // INTERFACE_P_H
