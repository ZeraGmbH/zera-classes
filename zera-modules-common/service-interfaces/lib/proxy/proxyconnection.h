#ifndef PROXYCONNECTION_H
#define PROXYCONNECTION_H

#include "proxynetpeer.h"
#include <QByteArray>

namespace Zera {

struct ProxyConnection
{
    ProxyConnection(QString ip, quint16 port, QByteArray uuid, ProxyNetPeer* client)
        :m_sIP(ip), m_nPort(port), m_binUUID(uuid), m_pNetClient(client){}
    QString m_sIP;
    quint16 m_nPort;
    QByteArray m_binUUID;
    ProxyNetPeer *m_pNetClient;
};

}

#endif // PROXYCONNECTION_H
