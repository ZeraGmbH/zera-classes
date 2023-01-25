#include "tcpserverlimitedconn.h"
#include <QTcpSocket>

namespace SCPIMODULE
{
TcpServerLimitedConn::TcpServerLimitedConn(QObject *parent)
    : QTcpServer{parent}
{
}

void TcpServerLimitedConn::incomingConnection(qintptr socketDescriptor)
{
    qInfo("new function out");
    if(m_SCPIClientList->count() < 5) {
        QTcpSocket *socket = new QTcpSocket(this);
        socket->setSocketDescriptor(socketDescriptor);
        addPendingConnection(socket);
        qInfo("new function in, %i",m_SCPIClientList->count());
        }
}
void TcpServerLimitedConn::setClientList(QList<cSCPIClient *> *SCPIClientList)
{
    m_SCPIClientList = SCPIClientList;
}
}
