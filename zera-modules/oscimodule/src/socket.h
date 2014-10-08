#ifndef SOCKET_H
#define SOCKET_H

#include <QObject>
#include <QString>

namespace OSCIMODULE
{

struct cSocket
{
    QString m_sIP;
    quint16 m_nPort;
};

}
#endif // SOCKET_H
