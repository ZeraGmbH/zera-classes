#ifndef PROXY_P_H
#define PROXY_P_H

#include "proxy.h"
#include <xiqnetpeer.h>
#include <xiqnetwrapper.h>
#include <QHash>
#include <QString>

namespace Zera {

class Proxy;
class ProxyClient;
class ProxyClientPrivate;
class ProxyConnection;
class ProxyNetPeer;

class ProxyPrivate: public QObject
{
    Q_OBJECT
public:
    quint32 transmitCommand(ProxyClientPrivate *client, ProtobufMessage::NetMessage *message);
protected:
    ProxyPrivate(Proxy *parent);
    ~ProxyPrivate(){}
    ProxyClient* getConnection(QString ipadress, quint16 port);
    ProxyClientPtr getConnectionSmart(QString ipadress, quint16 port);
    void startConnection(ProxyClientPrivate *client);
    bool releaseConnection(ProxyClientPrivate *client);
    static Proxy* singletonInstance;
    Proxy *q_ptr;
protected slots:
    void receiveMessage(std::shared_ptr<google::protobuf::Message> message);
    void receiveTcpError(QAbstractSocket::SocketError errorCode);
    void registerConnection();
    void registerDisConnection();
private:
    ProxyNetPeer *getProxyNetPeer(QString ipadress, quint16 port);
    ProxyNetPeer *searchConnection(QString ip, quint16 port); // we search for a netclient that matches ip, port
    XiQNetWrapper protobufWrapper;
    QHash<ProxyClientPrivate*, ProxyConnection*> m_ConnectionHash; // holds network connection for each client
    QHash<QByteArray, ProxyClientPrivate*> m_ClientHash; // information for faster redirecting
    quint32 m_nMessageNumber; // message number, .. we never use 0

    Q_DISABLE_COPY(ProxyPrivate)
    Q_DECLARE_PUBLIC(Proxy)
};

}

#endif // PROXY_P_H
