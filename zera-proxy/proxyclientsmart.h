#ifndef PROXYCLIENTSMART_H
#define PROXYCLIENTSMART_H

#include <QObject>
#include <QAbstractSocket>

namespace ProtobufMessage
{
    class NetMessage;
}

namespace Zera
{
namespace Proxy
{

class ProxyClientSmart : public QObject
{
    Q_OBJECT
public:
    virtual ~ProxyClientSmart();

signals:
    void answerAvailable(std::shared_ptr<ProtobufMessage::NetMessage>);
    void tcpError(QAbstractSocket::SocketError errorCode);
    void disconnected();
    void connected();
};

}
}
#endif // PROXYCLIENTSMART_H
