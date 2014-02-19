#ifndef PROXYCLIENT_P_H
#define PROXYCLIENT_P_H

#include <QObject>
#include <netmessages.pb.h>
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
    void transmitAnswer(ProtobufMessage::NetMessage *message);
    void transmitError(QAbstractSocket::SocketError errorCode);

protected:
    virtual quint32 transmitCommand(ProtobufMessage::NetMessage *message);
    cProxyPrivate *m_pProxy;

};

}
}
#endif // PROXYCLIENT_P_H
