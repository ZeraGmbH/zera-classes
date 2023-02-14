#ifndef PROXYCLIENT_H
#define PROXYCLIENT_H

#include "service-interfaces_export.h"
#include <netmessages.pb.h>
#include <QAbstractSocket>
#include <memory>

namespace Zera {

class ProxyClientPrivate;

class SERVICE_INTERFACES_EXPORT ProxyClient : public QObject
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

typedef std::shared_ptr<ProxyClient> ProxyClientPtr;

}

#endif // PROXYCLIENT_H
