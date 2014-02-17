#ifndef PROXICONNECTION_H
#define PROXICONNECTION_H

#include <QObject>
#include <QByteArray>
#include <zeraclientnetbase.h>

namespace Zera
{
namespace Proxi
{

struct cProxiConnection
{
    cProxiConnection(QString ip, quint16 port, QByteArray uuid, Zera::NetClient::cClientNetBase *client)
        :m_sIP(ip), m_nPort(port), m_binUUID(uuid), m_pNetClient(client){}
    QString m_sIP;
    quint16 m_nPort;
    QByteArray m_binUUID;
    Zera::NetClient::cClientNetBase *m_pNetClient;
};

}
}

#endif // PROXICONNECTION_H
