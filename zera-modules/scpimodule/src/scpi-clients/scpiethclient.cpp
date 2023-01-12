#include "scpiethclient.h"
#include <QHostAddress>

namespace SCPIMODULE {

cSCPIEthClient::cSCPIEthClient(QTcpSocket *socket, cSCPIModule *module, cSCPIModuleConfigData &configdata, cSCPIInterface *iface) :
    cSCPIClient(module, configdata, iface),
    m_pSocket(socket)
{
    m_peerAddress = getPeerAddress();
    qInfo("Incoming SCPI socket from %s", qPrintable(m_peerAddress));
    connect(m_pSocket, &QTcpSocket::readyRead, this, &cSCPIEthClient::cmdInput);
    connect(m_pSocket, &QTcpSocket::disconnected, this, &cSCPIEthClient::onDisconnect);
}

cSCPIEthClient::~cSCPIEthClient()
{
    m_pSocket->abort();
    delete m_pSocket;
    qInfo("SCPI socket %s deleted", qPrintable(m_peerAddress));
}

void cSCPIEthClient::receiveAnswer(QString answ)
{
    QByteArray ba = answ.toLatin1() + "\n";
    m_pSocket->write(ba);
}

QString cSCPIEthClient::getPeerAddress()
{
    QHostAddress addr = m_pSocket->peerAddress();
    return addr.toString();
}

void cSCPIEthClient::cmdInput()
{
    while (m_pSocket->canReadLine()) {
        QString m_sInput = m_pSocket->readLine();
        m_sInputFifo.append(m_sInput);
    }
    testCmd();
}

void cSCPIEthClient::onDisconnect()
{
    qInfo("SCPI socket %s disconnected", qPrintable(m_peerAddress));
    deleteLater();
}

}
