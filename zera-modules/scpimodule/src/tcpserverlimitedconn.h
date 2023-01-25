#ifndef TCPSERVERLIMITEDCONN_H
#define TCPSERVERLIMITEDCONN_H

#include "scpiclient.h"
#include "scpimoduleconfigdata.h"
#include <QTcpServer>

namespace SCPIMODULE
{
class TcpServerLimitedConn : public QTcpServer
{
public:
    explicit TcpServerLimitedConn(cSCPIModuleConfigData &configData, QList<cSCPIClient *> &scpiClientList);
    void incomingConnection(qintptr socketDescriptor) override;

private:
    cSCPIModuleConfigData &m_configData;
    QList<cSCPIClient *> &m_SCPIClientList;
};
}
#endif // TCPSERVERLIMITEDCONN_H
