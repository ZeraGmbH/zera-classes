#include "proxyclient_p.h"
#include "proxyclient.h"
#include "proxy.h"
#include "proxy_p.h"
#include <netmessages.pb.h>

namespace Zera
{
namespace Proxy
{

cProxyClientPrivate::cProxyClientPrivate(cProxyPrivate* proxy)
    :m_pProxy(proxy)
{
    setParent(proxy);
}


void cProxyClientPrivate::transmitAnswer(std::shared_ptr<ProtobufMessage::NetMessage> message)
{
    emit answerAvailable(message);
}


void cProxyClientPrivate::transmitError(QAbstractSocket::SocketError errorCode)
{
    emit tcpError(errorCode);
}


void cProxyClientPrivate::transmitConnection()
{
    emit connected();
}


void cProxyClientPrivate::transmitDisConnection()
{
    emit disconnected();
}


quint32 cProxyClientPrivate::transmitCommand(ProtobufMessage::NetMessage *message)
{
    return m_pProxy->transmitCommand(this, message);
}

}
}



