#include "proxiclient_p.h"
#include "proxiclient.h"
#include "proxi.h"
#include "proxi_p.h"

namespace Zera
{
namespace Proxi
{

cProxiClientPrivate::cProxiClientPrivate(cProxiPrivate* proxi)
    :m_pProxi(proxi)
{
    setParent(proxi);
}


void cProxiClientPrivate::transmitAnswer(ProtobufMessage::NetMessage *message)
{
    emit answerAvailable(message);
}


void cProxiClientPrivate::transmitError(QAbstractSocket::SocketError errorCode)
{
    emit tcpError(errorCode);
}


quint32 cProxiClientPrivate::transmitCommand(ProtobufMessage::NetMessage *message)
{
    return m_pProxi->transmitCommand(this, message);
}

}
}



