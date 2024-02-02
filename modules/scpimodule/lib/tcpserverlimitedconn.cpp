#include "tcpserverlimitedconn.h"
#include <QTcpSocket>

namespace SCPIMODULE
{
TcpServerLimitedConn::TcpServerLimitedConn(cSCPIModuleConfigData &configData, QList<cSCPIClient *> &scpiClientList) :
    m_configData(configData), m_SCPIClientList(scpiClientList)
{
}

void TcpServerLimitedConn::incomingConnection(qintptr socketDescriptor)
{
    QTcpSocket *socket = new QTcpSocket(this);
    socket->setSocketDescriptor(socketDescriptor);

    if(m_SCPIClientList.count() < m_configData.m_nClients) {
        qInfo("Add pending connection, %i.", m_SCPIClientList.count());
        addPendingConnection(socket);
    }
    else
    {
        qInfo("Close incoming connection. List of active connections is full (%i elements).", m_SCPIClientList.count());
        socket->close();
    }
}
}
