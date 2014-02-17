#include <QUuid>
#include <zeraclientnetbase.h>

#include "proxi_p.h"
#include "proxiclient_p.h"
#include "proxiconnection.h"

namespace Zera
{
namespace Proxi
{


cProxi* cProxiPrivate::singletonInstance=0;


cProxiPrivate::cProxiPrivate(cProxi *parent):
    q_ptr(parent)
{
    m_sIPAdress = "127.0.0.1"; // our default
    m_nMessageNumber = 0;
}


cProxiClient* cProxiPrivate::getConnection(QString ipadress, quint16 port)
{
    Zera::NetClient::cClientNetBase* netClient;
    QUuid uuid;
    QByteArray binUUid;

    if (ipadress == "")
        ipadress = m_sIPAdress;

    if ((netClient = searchConnection(ipadress, port)) == 0) // look for existing connection
    {
        // if not existing we have to create
        netClient = new Zera::NetClient::cClientNetBase(this);
        connect(netClient, SIGNAL(messageAvailable(QByteArray)), this, SLOT(receiveMessage(QByteArray)));
        connect(netClient, SIGNAL(tcpError(QAbstractSocket::SocketError)), this, SLOT(receiveTcpError(QAbstractSocket::SocketError)));
        netClient->startNetwork(ipadress, port);
    }

    cProxiClientPrivate *proxiclient = new cProxiClientPrivate(this);

    uuid.createUuid(); // we use a per client uuid
    binUUid = uuid.toRfc4122();

    cProxiConnection *connection = new cProxiConnection(ipadress, port, binUUid, netClient);
    m_ConnectionHash[proxiclient] = connection;
    m_ClientHash[binUUid] = proxiclient;

    return proxiclient;
}


cProxiClient* cProxiPrivate::getConnection(quint16 port)
{
    return getConnection(m_sIPAdress, port);
}


quint32 cProxiPrivate::transmitCommand(cProxiClientPrivate* client, ProtobufMessage::NetMessage *message)
{
    quint32 nr;

    message->set_clientid(m_ConnectionHash[client]->m_binUUID); // we put the client's id into message

    if (m_nMessageNumber == 0) m_nMessageNumber++; // we never use 0 as message number (reserved)
    message->set_messagenr(nr = m_nMessageNumber);
    m_nMessageNumber++; // increment message number

    m_ConnectionHash[client]->m_pNetClient->sendMessage(message);
    return nr;
}


void cProxiPrivate::setIPAdress(QString ipAddress)
{
    m_sIPAdress = ipAddress;
}


void cProxiPrivate::receiveMessage(QByteArray message)
{
    QHash<cProxiClientPrivate*, cProxiConnection*>::Iterator it;

    ProtobufMessage::NetMessage *netMessage = new ProtobufMessage::NetMessage();
    it = m_ConnectionHash.begin(); // we take first interface to "read" the data

    it.value()->m_pNetClient->readMessage(netMessage, message);

    QByteArray key(netMessage->clientid().c_str(), netMessage->clientid().size());
    if (m_ClientHash.contains(key))
    {
        m_ClientHash[key]->transmitAnswer(netMessage);
    }
    // ? error handling in case of unknown clientid ?
}


void cProxiPrivate::receiveTcpError(QAbstractSocket::SocketError errorCode)
{
    Zera::NetClient::cClientNetBase* netClient = qobject_cast<Zera::NetClient::cClientNetBase*>(QObject::sender());

    QHashIterator<cProxiClientPrivate*, cProxiConnection*> it(m_ConnectionHash);

    while (it.hasNext())
    {
        cProxiConnection *pC;
        pC = it.value();
        if (pC->m_pNetClient == netClient) // we found a client that was connected to netclient
        {
            cProxiClientPrivate* client = it.key();
            client->transmitError(errorCode); // so this client will forward error
        }
    }
}


Zera::NetClient::cClientNetBase* cProxiPrivate::searchConnection(QString ip, quint16 port)
{
    Zera::NetClient::cClientNetBase* lnetClient = 0;
    QHashIterator<cProxiClientPrivate*, cProxiConnection*> it(m_ConnectionHash);

    while (it.hasNext())
    {
        cProxiConnection *pC;
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
