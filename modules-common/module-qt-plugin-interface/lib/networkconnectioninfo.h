#ifndef NETWORKCONNECTIONINFO_H
#define NETWORKCONNECTIONINFO_H

#include <QString>

struct NetworkConnectionInfo
{
    NetworkConnectionInfo() = default;
    NetworkConnectionInfo(QString ip, quint16 port) :
        m_sIP(ip),
        m_nPort(port)
    {
    }
    QString m_sIP;
    quint16 m_nPort;
};

#endif // NETWORKCONNECTIONINFO_H
