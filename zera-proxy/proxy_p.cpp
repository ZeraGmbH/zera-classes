#include "proxy_p.h"
#include "proxyclient_p.h"
#include "proxynetpeer.h"
#include "proxyconnection.h"
#include <QUuid>
#include <QTimer>
#include <xiqnetpeer.h>
#include <netmessages.pb.h>

namespace Zera
{
namespace Proxy
{

cProxy* cProxyPrivate::singletonInstance=0;

cProxyPrivate::cProxyPrivate(cProxy *parent):
    q_ptr(parent)
{
    m_nMessageNumber = 0;
}

cProxyClient* cProxyPrivate::getConnection(QString ipadress, quint16 port)
{
    cProxyNetPeer* netClient = searchConnection(ipadress, port);
    if(!netClient)  {// look for existing connection
        netClient = new cProxyNetPeer(this);
        netClient->setWrapper(&protobufWrapper);
        connect(netClient, &cProxyNetPeer::sigMessageReceived, this, &cProxyPrivate::receiveMessage);
        connect(netClient, &cProxyNetPeer::sigSocketError, this, &cProxyPrivate::receiveTcpError);
        connect(netClient, &cProxyNetPeer::sigConnectionEstablished, this, &cProxyPrivate::registerConnection);
        connect(netClient, &cProxyNetPeer::sigConnectionClosed, this, &cProxyPrivate::registerDisConnection);
    }

    cProxyClientPrivate *proxyclient = new cProxyClientPrivate(this);

    QUuid uuid = QUuid::createUuid(); // we use a per client uuid
    QByteArray binUUid = uuid.toRfc4122();

    cProxyConnection *connection = new cProxyConnection(ipadress, port, binUUid, netClient);
    m_ConnectionHash[proxyclient] = connection;
    m_ClientHash[binUUid] = proxyclient;

    return proxyclient;
}

void cProxyPrivate::startConnection(cProxyClientPrivate *client)
{
    cProxyConnection *connection = m_ConnectionHash[client];
    cProxyNetPeer *peer = connection->m_pNetClient;
    if(peer->isStarted()) {
        if(peer->isConnected())
            client->transmitConnection();
    }
    else
        peer->startProxyConnection(connection->m_sIP, connection->m_nPort);
}

bool cProxyPrivate::releaseConnection(cProxyClientPrivate *client)
{
    if(m_ConnectionHash.contains(client)) {
        cProxyConnection *connection = m_ConnectionHash.take(client);
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


quint32 cProxyPrivate::transmitCommand(cProxyClientPrivate* client, ProtobufMessage::NetMessage *message)
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

void cProxyPrivate::receiveMessage(std::shared_ptr<google::protobuf::Message> message)
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

void cProxyPrivate::receiveTcpError(QAbstractSocket::SocketError errorCode)
{
    cProxyNetPeer* netClient = qobject_cast<cProxyNetPeer*>(QObject::sender());
    QHashIterator<cProxyClientPrivate*, cProxyConnection*> it(m_ConnectionHash);
    while(it.hasNext()) {
        it.next();
        cProxyConnection *pC = it.value();
        if(pC->m_pNetClient == netClient) { // we found a client that was connected to netclient
            cProxyClientPrivate* client = it.key();
            client->transmitError(errorCode); // so this client will forward error
        }
    }
}

void cProxyPrivate::registerConnection()
{
    cProxyNetPeer* netClient = qobject_cast<cProxyNetPeer*>(QObject::sender());
    QHashIterator<cProxyClientPrivate*, cProxyConnection*> it(m_ConnectionHash);
    while(it.hasNext()) {
        it.next();
        cProxyConnection *pC = it.value();
        if(pC->m_pNetClient == netClient) { // we found a client that tried to connected to netclient
            cProxyClientPrivate* client = it.key();
            client->transmitConnection();
        }
    }
}

void cProxyPrivate::registerDisConnection()
{
    cProxyNetPeer* netClient = qobject_cast<cProxyNetPeer*>(QObject::sender());
    QHashIterator<cProxyClientPrivate*, cProxyConnection*> it(m_ConnectionHash);
    while(it.hasNext()) {
        it.next();
        cProxyConnection *pC = it.value();
        if(pC->m_pNetClient == netClient) { // we found a client that tried to connected to netclient
            cProxyClientPrivate* client = it.key();
            client->transmitDisConnection(); // so this client will be forwarded connection
        }
    }
}

cProxyNetPeer* cProxyPrivate::searchConnection(QString ip, quint16 port)
{
    cProxyNetPeer* lnetClient = nullptr;
    QHashIterator<cProxyClientPrivate*, cProxyConnection*> it(m_ConnectionHash);
    while(it.hasNext()) {
        it.next();
        cProxyConnection *pC = it.value();
        if( pC->m_sIP == ip && pC->m_nPort == port) {
             lnetClient = pC->m_pNetClient;
             break;
        }
    }
    return lnetClient;
}

}
}
