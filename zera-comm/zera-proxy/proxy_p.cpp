#include <QUuid>
#include <QTimer>
#include <protonetpeer.h>
#include <netmessages.pb.h>

#include "proxy_p.h"
#include "proxyclient_p.h"
#include "proxyconnection.h"


namespace Zera
{
namespace Proxy
{

cProxy* cProxyPrivate::singletonInstance=0;


cProxyPrivate::cProxyPrivate(cProxy *parent):
    q_ptr(parent)
{
    m_sIPAdress = "127.0.0.1"; // our default
    m_nMessageNumber = 0;
}


cProxyClient* cProxyPrivate::getConnection(QString ipadress, quint16 port)
{
    bool existed;
    ProtoNetPeer* netClient;
    QUuid uuid;
    QByteArray binUUid;

    if (ipadress == "")
        ipadress = m_sIPAdress;

    if ((netClient = searchConnection(ipadress, port)) == 0) // look for existing connection
    {
        existed = false;
        // if not existing we have to create
        netClient = new ProtoNetPeer(this);
        netClient->setWrapper(&protobufWrapper);
        connect(netClient, SIGNAL(sigMessageReceived(google::protobuf::Message*)), this, SLOT(receiveMessage(google::protobuf::Message*)));
        connect(netClient, SIGNAL(sigSocketError(QAbstractSocket::SocketError)), this, SLOT(receiveTcpError(QAbstractSocket::SocketError)));
        connect(netClient, SIGNAL(sigConnectionEstablished()), this, SLOT(registerConnection()));
        connect(netClient, SIGNAL(sigConnectionClosed()), this, SLOT(registerDisConnection()));
        netClient->startConnection(ipadress, port);
    }
    else
        existed = true;

    cProxyClientPrivate *proxyclient = new cProxyClientPrivate(this);

    uuid = QUuid::createUuid(); // we use a per client uuid
    binUUid = uuid.toRfc4122();

    cProxyConnection *connection = new cProxyConnection(ipadress, port, binUUid, netClient);
    m_ConnectionHash[proxyclient] = connection;
    m_ClientHash[binUUid] = proxyclient;

    if (existed && netClient->isConnected()) // if the wanted neclient already existed and!!! connected
        QTimer::singleShot(10, proxyclient, SLOT(transmitConnection()));

    return proxyclient;
}


cProxyClient* cProxyPrivate::getConnection(quint16 port)
{
    return getConnection(m_sIPAdress, port);
}


bool cProxyPrivate::releaseConnection(cProxyClientPrivate *client)
{
    if (m_ConnectionHash.contains(client))
    {
        cProxyConnection *connection = m_ConnectionHash.take(client);
        QByteArray binUUid = m_ClientHash.key(client);
        m_ClientHash.remove(binUUid);

        ProtobufMessage::NetMessage netCommand;
        ProtobufMessage::NetMessage::NetCmd *Command = netCommand.mutable_netcommand();
        Command->set_cmd(ProtobufMessage::NetMessage_NetCmd_CmdType_RELEASE);
        netCommand.set_clientid(binUUid.data(), binUUid.size());
        connection->m_pNetClient->sendMessage(&netCommand);
        delete connection;
        return true;
    }

    return false;
}


quint32 cProxyPrivate::transmitCommand(cProxyClientPrivate* client, ProtobufMessage::NetMessage *message)
{
    quint32 nr;

    QByteArray ba = m_ConnectionHash[client]->m_binUUID;
    message->set_clientid(ba.data(), ba.size()); // we put the client's id into message

    if (m_nMessageNumber == 0) m_nMessageNumber++; // we never use 0 as message number (reserved)
    message->set_messagenr(nr = m_nMessageNumber);
    m_nMessageNumber++; // increment message number

    m_ConnectionHash[client]->m_pNetClient->sendMessage(message);
    return nr;
}


void cProxyPrivate::setIPAdress(QString ipAddress)
{
    m_sIPAdress = ipAddress;
}


void cProxyPrivate::receiveMessage(google::protobuf::Message* message)
{
    ProtobufMessage::NetMessage *netMessage = static_cast<ProtobufMessage::NetMessage*>(message);

    if (netMessage->has_clientid())
    {
        QByteArray key(netMessage->clientid().data(), netMessage->clientid().size());
        if (m_ClientHash.contains(key))
        {
            m_ClientHash[key]->transmitAnswer(netMessage);
        }
    }
    // ? todo error handling in case of unknown clientid ?
}


void cProxyPrivate::receiveTcpError(QAbstractSocket::SocketError errorCode)
{
    ProtoNetPeer* netClient = qobject_cast<ProtoNetPeer*>(QObject::sender());

    QHashIterator<cProxyClientPrivate*, cProxyConnection*> it(m_ConnectionHash);

    while (it.hasNext())
    {
        cProxyConnection *pC;
        it.next();
        pC = it.value();
        if (pC->m_pNetClient == netClient) // we found a client that was connected to netclient
        {
            cProxyClientPrivate* client = it.key();
            client->transmitError(errorCode); // so this client will forward error
        }
    }
}


void cProxyPrivate::registerConnection()
{
    ProtoNetPeer* netClient = qobject_cast<ProtoNetPeer*>(QObject::sender());

    QHashIterator<cProxyClientPrivate*, cProxyConnection*> it(m_ConnectionHash);

    while (it.hasNext())
    {
        cProxyConnection *pC;
        it.next();
        pC = it.value();
        if (pC->m_pNetClient == netClient) // we found a client that tried to connected to netclient
        {
            cProxyClientPrivate* client = it.key();
            QTimer::singleShot(10, client, SLOT(transmitConnection()));
            // client->transmitConnection(); // so this client will be forwarded connection
            // direct call is to fast, so we take a timer
            // todo: split start connection from getconnection
        }
    }
}

void cProxyPrivate::registerDisConnection()
{
    ProtoNetPeer* netClient = qobject_cast<ProtoNetPeer*>(QObject::sender());

    QHashIterator<cProxyClientPrivate*, cProxyConnection*> it(m_ConnectionHash);

    while (it.hasNext())
    {
        cProxyConnection *pC;
        it.next();
        pC = it.value();
        if (pC->m_pNetClient == netClient) // we found a client that tried to connected to netclient
        {
            cProxyClientPrivate* client = it.key();
            client->transmitDisConnection(); // so this client will be forwarded connection
        }
    }
}


ProtoNetPeer* cProxyPrivate::searchConnection(QString ip, quint16 port)
{
    ProtoNetPeer* lnetClient = 0;
    QHashIterator<cProxyClientPrivate*, cProxyConnection*> it(m_ConnectionHash);

    while (it.hasNext())
    {
        cProxyConnection *pC;
        it.next();
        pC = it.value();
        if ( (pC->m_sIP == ip) && (pC->m_nPort == port))
        {
             lnetClient = pC->m_pNetClient;
             break;
        }
    }

    return lnetClient;
}

}
}
