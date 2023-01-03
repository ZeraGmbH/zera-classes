#ifndef PROXY_P_H
#define PROXY_P_H

#include "proxy.h"
#include <netmessages.pb.h>
#include <xiqnetpeer.h>
#include <xiqnetwrapper.h>
#include <QHash>
#include <QString>

namespace Zera { namespace Proxy {

class cProxy;
class cProxyClient;
class cProxyClientPrivate;
class cProxyConnection;
class cProxyNetPeer;

class cProxyPrivate: public QObject
{
    Q_OBJECT
public:
    quint32 transmitCommand(cProxyClientPrivate *client, ProtobufMessage::NetMessage *message);
protected:
    cProxyPrivate(cProxy *parent);
    ~cProxyPrivate(){}
    cProxyClient* getConnection(QString ipadress, quint16 port);
    ProxyClientPtr getConnectionSmart(QString ipadress, quint16 port);
    void startConnection(cProxyClientPrivate *client);
    bool releaseConnection(cProxyClientPrivate *client);
    static cProxy* singletonInstance;
    cProxy *q_ptr;
protected slots:
    void receiveMessage(std::shared_ptr<google::protobuf::Message> message);
    void receiveTcpError(QAbstractSocket::SocketError errorCode);
    void registerConnection();
    void registerDisConnection();
private:
    cProxyNetPeer *getProxyNetPeer(QString ipadress, quint16 port);
    cProxyNetPeer *searchConnection(QString ip, quint16 port); // we search for a netclient that matches ip, port
    XiQNetWrapper protobufWrapper;
    QHash<cProxyClientPrivate*, cProxyConnection*> m_ConnectionHash; // holds network connection for each client
    QHash<QByteArray, cProxyClientPrivate*> m_ClientHash; // information for faster redirecting
    quint32 m_nMessageNumber; // message number, .. we never use 0

    Q_DISABLE_COPY(cProxyPrivate)
    Q_DECLARE_PUBLIC(cProxy)
};

} }



#endif // PROXY_P_H
