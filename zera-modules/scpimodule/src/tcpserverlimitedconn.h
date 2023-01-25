#ifndef TCPSERVERLIMITEDCONN_H
#define TCPSERVERLIMITEDCONN_H

#include "scpiclient.h"
#include <QTcpServer>

namespace SCPIMODULE
{
class TcpServerLimitedConn : public QTcpServer
{
public:
    explicit TcpServerLimitedConn(QObject *parent = nullptr);
    void incomingConnection(qintptr socketDescriptor) override;
    void setClientList(QList<cSCPIClient *> *SCPIClientList);

private:
    QList<cSCPIClient *> *m_SCPIClientList;
};
}
#endif // TCPSERVERLIMITEDCONN_H
