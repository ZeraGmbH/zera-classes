#ifndef PROXYCONNECTION_H
#define PROXYCONNECTION_H

#include <QObject>
#include <QByteArray>

class ProtoNetPeer;

namespace Zera
{
namespace Proxy
{

struct cProxyConnection
{
    cProxyConnection(QString ip, quint16 port, QByteArray uuid, ProtoNetPeer* client)
        :m_sIP(ip), m_nPort(port), m_binUUID(uuid), m_pNetClient(client){}
    QString m_sIP;
    quint16 m_nPort;
    QByteArray m_binUUID;
    ProtoNetPeer *m_pNetClient;
};

}
}

#endif // PROXYCONNECTION_H
