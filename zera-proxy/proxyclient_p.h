#ifndef PROXYCLIENT_P_H
#define PROXYCLIENT_P_H

#include <QObject>
#include <proxyclient.h>


namespace Zera
{
namespace Proxy
{

class cProxyPrivate;

class cProxyClientPrivate: public cProxyClient
{
    Q_OBJECT

public:
    cProxyClientPrivate(cProxyPrivate* proxy);
    virtual ~cProxyClientPrivate();
    void transmitAnswer(std::shared_ptr<ProtobufMessage::NetMessage> message);
    void transmitError(QAbstractSocket::SocketError errorCode);
    void transmitDisConnection();

public slots:
    void transmitConnection();

protected:
    virtual quint32 transmitCommand(ProtobufMessage::NetMessage *message);
    cProxyPrivate *m_pProxy;

};

}
}
#endif // PROXYCLIENT_P_H
