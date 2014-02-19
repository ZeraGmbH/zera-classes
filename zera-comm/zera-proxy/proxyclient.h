#ifndef PROXICLIENT_H
#define PROXICLIENT_H

#include <QObject>
#include <QAbstractSocket>
#include <netmessages.pb.h>

#include "proxi_global.h"


namespace Zera
{
namespace Proxi
{

class cProxiClientPrivate;


class ZERAPROXISHARED_EXPORT cProxiClient: public QObject
{
    Q_OBJECT

public:
    /**
     * @brief transmitCommand
     * @param message
     * @return value is message number for this message, can be used for sync.
     */
    virtual quint32 transmitCommand(ProtobufMessage::NetMessage *message) = 0;

signals:
    void answerAvailable(ProtobufMessage::NetMessage*);
    void tcpError(QAbstractSocket::SocketError errorCode);
};

}
}

#endif // PROXICLIENT_H
