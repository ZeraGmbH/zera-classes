#ifndef INTERFACE_P_H
#define INTERFACE_P_H

#include <QObject>
#include <QAbstractSocket>
#include <QHash>
#include <QVariant>
#include <proxiclient.h>
#include <netmessages.pb.h>

#include "zera_misc_global.h"

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
    quint32 sendCommand(QString cmd);
    quint32 sendCommand(QString cmd, QString par);

    QVariant returnInt(QString message);
    QVariant returnString( QString message);
    QVariant returnStringList(QString message);
    QVariant returnDouble(QString message);
    QVariant returnBool(QString message);
    QVariant returnReply(int reply);

    QHash<quint32, int> m_MsgNrCmdList;
    Zera::Proxi::cProxiClient *m_pClient;

protected slots:
    virtual void receiveAnswer(ProtobufMessage::NetMessage *message) = 0;
    virtual void receiveError(QAbstractSocket::SocketError errorCode) = 0;
};

}
}

#endif // INTERFACE_P_H
