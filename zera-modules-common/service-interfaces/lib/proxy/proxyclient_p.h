#ifndef PROXYCLIENT_P_H
#define PROXYCLIENT_P_H

#include <QObject>
#include <proxyclient.h>

namespace Zera { namespace Proxy {

class ProxyPrivate;

class ProxyClientPrivate: public ProxyClient
{
    Q_OBJECT
public:
    ProxyClientPrivate(ProxyPrivate* proxy);
    virtual ~ProxyClientPrivate();
    void transmitAnswer(std::shared_ptr<ProtobufMessage::NetMessage> message);
    void transmitError(QAbstractSocket::SocketError errorCode);
    void transmitDisConnection();
public slots:
    void transmitConnection();
protected:
    virtual quint32 transmitCommand(ProtobufMessage::NetMessage *message);
    ProxyPrivate *m_pProxy;
};

}}
#endif // PROXYCLIENT_P_H
