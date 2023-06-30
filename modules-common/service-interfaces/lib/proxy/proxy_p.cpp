#include "proxy_p.h"
#include "proxyclient_p.h"
#include "proxynetpeer.h"
#include "proxyconnection.h"
#include <QUuid>
#include <QTimer>
#include <xiqnetpeer.h>
#include <netmessages.pb.h>

namespace Zera {

Proxy* ProxyPrivate::singletonInstance=0;

ProxyPrivate::ProxyPrivate(Proxy *parent):
    q_ptr(parent)
{
    m_nMessageNumber = 0;
}

ProxyClient* ProxyPrivate::getConnection(QString ipadress, quint16 port)
{
    QUuid uuid = QUuid::createUuid(); // we use a per client uuid
    QByteArray binUUid = uuid.toRfc4122();

    ProxyNetPeer *proxyPeer = getProxyNetPeer(ipadress, port);
    ProxyClientPrivate *proxyclient = new ProxyClientPrivate(this);
    ProxyConnection *connection = new ProxyConnection(ipadress, port, binUUid, proxyPeer);
    m_ConnectionHash[proxyclient] = connection;
    m_ClientHash[binUUid] = proxyclient;

    return proxyclient;
}

ProxyClientPtr ProxyPrivate::getConnectionSmart(QString ipadress, quint16 port)
{
    ProxyClient* client = getConnection(ipadress, port);
    return ProxyClientPtr(client);
}

void ProxyPrivate::startConnection(ProxyClientPrivate *client)
{
    ProxyConnection *connection = m_ConnectionHash[client];
    ProxyNetPeer *peer = connection->m_pNetClient;
    if(peer->isStarted()) {
        if(peer->isConnected())
            client->transmitConnection();
    }
    else
        peer->startProxyConnection(connection->m_sIP, connection->m_nPort);
}

bool ProxyPrivate::releaseConnection(ProxyClientPrivate *client)
{
    if(m_ConnectionHash.contains(client)) {
        ProxyConnection *connection = m_ConnectionHash.take(client);
        QByteArray binUUid = m_ClientHash.key(client);
        m_ClientHash.remove(binUUid);

        ProtobufMessage::NetMessage netCommand;
        ProtobufMessage::NetMessage::NetCmd *Command = netCommand.mutable_netcommand();
        Command->set_cmd(ProtobufMessage::NetMessage_NetCmd_CmdType_RELEASE);
        netCommand.set_clientid(binUUid.data(), binUUid.size());
        connection->m_pNetClient->sendMessage(netCommand);
        delete connection;
        return true;
    }
    return false;
}

quint32 ProxyPrivate::transmitCommand(ProxyClientPrivate* client, ProtobufMessage::NetMessage *message)
{
    QByteArray ba = m_ConnectionHash[client]->m_binUUID;
    message->set_clientid(ba.data(), ba.size()); // we put the client's id into message

    if(m_nMessageNumber == 0)
        m_nMessageNumber++; // we never use 0 as message number (reserved)
    quint32 nr = m_nMessageNumber;
    message->set_messagenr(nr);
    m_nMessageNumber++; // increment message number

    m_ConnectionHash[client]->m_pNetClient->sendMessage(*message);
    return nr;
}

void ProxyPrivate::receiveMessage(std::shared_ptr<google::protobuf::Message> message)
{
    std::shared_ptr<ProtobufMessage::NetMessage> netMessage = std::static_pointer_cast<ProtobufMessage::NetMessage>(message);
    if(netMessage->has_clientid()) {
        QByteArray key(netMessage->clientid().data(), netMessage->clientid().size());
        if(m_ClientHash.contains(key))
            m_ClientHash[key]->transmitAnswer(netMessage);
        else
            qWarning() << "Unknown ClientID; " << netMessage->clientid().size();
    }
    // ? todo error handling in case of unknown clientid ?
    else
        qWarning() << "No ClientID";
}

void ProxyPrivate::receiveTcpError(QAbstractSocket::SocketError errorCode)
{
    ProxyNetPeer* netClient = qobject_cast<ProxyNetPeer*>(QObject::sender());
    QHashIterator<ProxyClientPrivate*, ProxyConnection*> it(m_ConnectionHash);
    while(it.hasNext()) {
        it.next();
        ProxyConnection *pC = it.value();
        if(pC->m_pNetClient == netClient) { // we found a client that was connected to netclient
            ProxyClientPrivate* client = it.key();
            client->transmitError(errorCode); // so this client will forward error
        }
    }
}

void ProxyPrivate::registerConnection()
{
    ProxyNetPeer* netClient = qobject_cast<ProxyNetPeer*>(QObject::sender());
    QHashIterator<ProxyClientPrivate*, ProxyConnection*> it(m_ConnectionHash);
    while(it.hasNext()) {
        it.next();
        ProxyConnection *pC = it.value();
        if(pC->m_pNetClient == netClient) { // we found a client that tried to connected to netclient
            ProxyClientPrivate* client = it.key();
            client->transmitConnection();
        }
    }
}

void ProxyPrivate::registerDisConnection()
{
    ProxyNetPeer* netClient = qobject_cast<ProxyNetPeer*>(QObject::sender());
    QHashIterator<ProxyClientPrivate*, ProxyConnection*> it(m_ConnectionHash);
    while(it.hasNext()) {
        it.next();
        ProxyConnection *pC = it.value();
        if(pC->m_pNetClient == netClient) { // we found a client that tried to connected to netclient
            ProxyClientPrivate* client = it.key();
            client->transmitDisConnection(); // so this client will be forwarded connection
        }
    }
}

ProxyNetPeer *ProxyPrivate::getProxyNetPeer(QString ipadress, quint16 port)
{
    ProxyNetPeer* netClient = searchConnection(ipadress, port);
    if(!netClient)  {// look for existing connection
        netClient = new ProxyNetPeer(this);
        netClient->setWrapper(&protobufWrapper);
        connect(netClient, &ProxyNetPeer::sigMessageReceived, this, &ProxyPrivate::receiveMessage);
        connect(netClient, &ProxyNetPeer::sigSocketError, this, &ProxyPrivate::receiveTcpError);
        connect(netClient, &ProxyNetPeer::sigConnectionEstablished, this, &ProxyPrivate::registerConnection);
        connect(netClient, &ProxyNetPeer::sigConnectionClosed, this, &ProxyPrivate::registerDisConnection);
    }
    return netClient;
}

ProxyNetPeer* ProxyPrivate::searchConnection(QString ip, quint16 port)
{
    ProxyNetPeer* lnetClient = nullptr;
    QHashIterator<ProxyClientPrivate*, ProxyConnection*> it(m_ConnectionHash);
    while(it.hasNext()) {
        it.next();
        ProxyConnection *pC = it.value();
        if( pC->m_sIP == ip && pC->m_nPort == port) {
             lnetClient = pC->m_pNetClient;
             break;
        }
    }
    return lnetClient;
}

}
