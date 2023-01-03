#ifndef INTERFACE_P_H
#define INTERFACE_P_H

#include <proxyclient.h>
#include <netmessages.pb.h>
#include <QObject>
#include <QAbstractSocket>
#include <QHash>
#include <QVariant>

namespace Zera
{
namespace Server
{

enum replies
{
    ack,
    nack,
    error,
    debug,
    ident
};


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

    QVariant returnInt(QString message);
    QVariant returnString( QString message);
    QVariant returnStringList(QString message);
    QVariant returnDouble(QString message);
    QVariant returnBool(QString message);

    QHash<quint32, int> m_MsgNrCmdList;
    Zera::Proxy::cProxyClient *m_pClient;
protected slots:
    virtual void receiveAnswer(std::shared_ptr<ProtobufMessage::NetMessage> message) = 0;
    virtual void receiveError(QAbstractSocket::SocketError errorCode) = 0;
};

}
}

#endif // INTERFACE_P_H
