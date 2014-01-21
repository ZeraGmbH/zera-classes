#include <QUuid>

#include "pcbproxy_p.h"
#include "interface_p.h"

namespace Zera
{
namespace PCBProxy
{


cPCBProxy* cPCBProxyPrivate::singletonInstance=0;


cPCBProxyPrivate::cPCBProxyPrivate(cPCBProxy *parent):
    q_ptr(parent)
{
    m_sIPAdress = "127.0.0.1"; // our default
}


cInterface *cPCBProxyPrivate::getInterface(quint16 port)
{
    if (!m_PCBServerHash.contains(port)) // we do not have a connection yet
    {
        Zera::NetClient::cClientNetBase* netclient = new Zera::NetClient::cClientNetBase(this);
        netclient->startNetwork(m_sIPAdress, port);
        m_PCBServerHash[port] = netclient;
    }

    Zera::NetClient::cClientNetBase* netclient = m_PCBServerHash[port];
    cInterfacePrivate* piface = new cInterfacePrivate(this);
    m_InterfaceHash[piface] = netclient;

    connect(piface, SIGNAL(pcbCommand(ProtobufMessage::NetMessage*)), this, SLOT(transferCommand(ProtobufMessage::NetMessage*)));
    connect(netclient, SIGNAL(messageAvailable(QByteArray)), this, SLOT(receiveMessage(QByteArray)));

    return piface;
}


void cPCBProxyPrivate::setIPAdress(QString ipAddress)
{
    m_sIPAdress = ipAddress;
}


void cPCBProxyPrivate::transferCommand(ProtobufMessage::NetMessage *message)
{
    QUuid uuid;
    QByteArray key;

    uuid.createUuid();
    key = uuid.toRfc4122();
    message->set_uuid(key);
    cInterfacePrivate* iface = qobject_cast<cInterfacePrivate*>(sender());
    m_UUIDHash[key] = iface;
    m_InterfaceHash[iface]->sendMessage(message);
}


void cPCBProxyPrivate::receiveMessage(QByteArray message)
{
    QHash<Zera::PCBProxy::cInterfacePrivate*, Zera::NetClient::cClientNetBase*>::Iterator it;

    ProtobufMessage::NetMessage *netMessage = new ProtobufMessage::NetMessage();
    it = m_InterfaceHash.begin(); // we take first interface to "read" the data

    it.value()->readMessage(netMessage, message);

    QByteArray key(netMessage->uuid().c_str(), netMessage->uuid().size());
    if (m_UUIDHash.contains(key))
    {
        Zera::PCBProxy::cInterfacePrivate* iface = m_UUIDHash.take(key);
        iface->transferAnswer(netMessage);
    }
}

}
}
