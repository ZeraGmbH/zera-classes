#ifndef PROXYCLIENT_H
#define PROXYCLIENT_H

#include <QObject>
#include <QAbstractSocket>

#include "proxy_global.h"

#include <memory>

namespace ProtobufMessage
{
    class NetMessage;
}


namespace Zera
{
namespace Proxy
{

class cProxyClientPrivate;


class ZERAPROXYSHARED_EXPORT cProxyClient: public QObject
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
    void answerAvailable(std::shared_ptr<ProtobufMessage::NetMessage>);
    void tcpError(QAbstractSocket::SocketError errorCode);
    void disconnected();
    void connected();
};

typedef std::shared_ptr<cProxyClient> ProxyClientPtr;

}
}

#endif // PROXYCLIENT_H
