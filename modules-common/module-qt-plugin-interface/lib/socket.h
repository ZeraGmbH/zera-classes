#ifndef SOCKET_H
#define SOCKET_H

#include <QString>

struct cSocket
{
    cSocket() = default;
    cSocket(QString ip, quint16 port) :
        m_sIP(ip),
        m_nPort(port)
    {
    }
    QString m_sIP;
    quint16 m_nPort;
};

#endif // SOCKET_H
